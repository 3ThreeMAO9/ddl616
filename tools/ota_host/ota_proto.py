#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
串口 OTA 上传脚本（带异或加密）

    python tools\\ota_tool\\ota_upload.py --port COM7 --file V39_140_010.bin

固件是工程根目录下带 16 字节头的 .bin（由 build_fw.py 生成）。

协议（doc/串口OTA升级对接说明_v1.1.md + 串口加密对接说明_v1.3.md）：
    A5 | attr | checksum1(2) | sn(2) | cmd | random | checksum2(2) | length(2) | payload

    加密：只加密 payload，attr bit4-5 = 1 表示异或
          实际密钥 k = random ^ K，逐字节异或
          checksum1 覆盖密文，checksum2 覆盖明文

    密钥分派：
        K_F = 0xBA            仅用于 0x70 / 0x71 / 0xF0 / 0xF1
        K_L = 会话密钥        由 4 字节密钥帧的下标 1 传递，其余帧全用它
        流程：发 0x71（用 K_F 加密）→ 设备回 0xF1，payload[1] 即 K_L
              设备也可能主动发 0x70 广播密钥，收到后回 0xF0

    0x72 开始 [size4][csum4][ver3]  → 0xF2 [status1][pkt_size2]
    0x73 数据 [offset4][data N]     → 0xF3 [status1][offset4]
    0x73 结束 [offset4]（N=0）      → 0xF3 + 0x74 [result1][ver3]
"""

import datetime
import os
import random
import re
import struct
import sys
import time

MARK = 0xA5
HEAD_SIZE = 12

CMD_KEY_SEND = 0x70
CMD_KEY_REQ = 0x71
ACK_KEY_SEND = CMD_KEY_SEND | 0x80     # 0xF0
ACK_KEY_REQ = CMD_KEY_REQ | 0x80       # 0xF1

CMD_OTA_START = 0x72
CMD_OTA_DATA = 0x73
CMD_OTA_RESULT = 0x74
ACK_START = CMD_OTA_START | 0x80       # 0xF2
ACK_DATA = CMD_OTA_DATA | 0x80         # 0xF3

KEY_FACTORY = 0xBA
KEY_INDEX = 1                          # 4 字节密钥帧取下标 1


def _decrypt_key_for_cmd(cmd, session_key):
    """密钥交换帧（0x70/0x71 及其应答）用 K_F，其余加密帧用会话密钥"""
    return KEY_FACTORY if (cmd & 0x7F) in (CMD_KEY_SEND, CMD_KEY_REQ) else session_key

OTA_MAGIC = 0xBEEF
OTA_HEADER_SIZE = 16

# 门锁的接收队列 UART_RX_QUEUE_SIZE = 256（环形缓冲可用 255），
# 一帧 = 12(头) + 4(offset) + pkt，包长必须 <= 239，否则整帧会被丢弃。
OTA_PKT_MAX = 239           # 受接收队列限制的实际上限
OTA_PKT_DEFAULT = 200       # 实际使用的包长

class OtaError(Exception):
    """升级过程出错"""


LOG_FP = None
LOG_SINK = None           # 额外的日志回调（GUI 用它刷新界面）

# 显示控制：默认只显示串口收发，不显示门锁自身的调试日志
SHOW_DEV_LOG = False
TRACE = True
LAST_VERSION = None          # 从门锁日志里解析出的 outside_version
OTA_SKIPPED = 2              # run_upload 返回值：版本相同，未执行升级
VERSION_RE = re.compile(r"outside_version:\s*V39\.140\.(\d+)")


def expected_patch(path):
    """从文件名 V39_140_005.bin 里取 PATCH_VERSION = 5，取不到返回 None"""
    m = re.search(r"_(\d{3})\.bin$", os.path.basename(path), re.IGNORECASE)
    return int(m.group(1)) if m else None


def log(msg=""):
    ts = datetime.datetime.now().strftime("%H:%M:%S.%f")[:-3]
    line = f"[{ts}] {msg}"
    # pythonw 启动时没有控制台，sys.stdout 为 None，只输出到日志文件和 GUI
    if sys.stdout is not None:
        try:
            sys.stdout.write(line + "\n")
            sys.stdout.flush()
        except Exception:
            pass
    if LOG_FP:
        LOG_FP.write(line + "\n")
        LOG_FP.flush()
    if LOG_SINK:
        try:
            LOG_SINK(line)
        except Exception:
            pass


def die(msg):
    log(f"OTA 失败：{msg}")
    raise OtaError(msg)


def open_log(log_dir=None):
    """日志目录可指定；不指定就不落盘（只输出到 stdout 和 GUI 的日志框）"""
    global LOG_FP
    if not log_dir:
        LOG_FP = None
        return
    d = log_dir
    os.makedirs(d, exist_ok=True)
    name = datetime.datetime.now().strftime("ota_%Y%m%d_%H%M%S.log")
    LOG_FP = open(os.path.join(d, name), "w", encoding="utf-8")


def send_frame(ser, cmd, payload, sn, key=None):
    """发一帧并打印 TX 信息"""
    data = build_frame(cmd, payload, sn, key)
    ser.write(data)
    if TRACE:
        extra = ""
        if (cmd & 0x7F) == CMD_OTA_DATA and len(payload) >= 4:
            extra = " off=0x%06X" % struct.unpack_from("<I", payload, 0)[0]
        log(f"  TX cmd=0x{cmd:02X} sn={sn} len={len(payload)}{extra}")
    return data


def checksum16(data):
    """帧头/数据校验，对应固件的 utils.c: check_sum()
    纯逐字节累加，不是 16 位字累加。"""
    return sum(data) & 0xFFFF


def checksum32(data):
    """固件内容校验，对应 ota_file_calc_checksum_u32：
    4 字节小端累加，尾部不足 4 字节按低位补"""
    n = len(data) & ~3
    s = 0
    for i in range(0, n, 4):
        s = (s + struct.unpack_from("<I", data, i)[0]) & 0xFFFFFFFF
    tail = data[n:]
    if tail:
        v = 0
        for i, b in enumerate(tail):
            v |= b << (8 * i)
        s = (s + v) & 0xFFFFFFFF
    return s


def xor_crypt(data, rnd, key):
    """k = random ^ key，逐字节异或（与固件 data_xor_crypt 一致）"""
    k = rnd ^ key
    return bytes(b ^ k for b in data)


def build_frame(cmd, payload, sn, key=None):
    """key=None 表示明文（enc=0）；否则用 k = R ^ key 加密 payload"""
    if key is None:
        attr = 0
        rnd = 0
        body_payload = payload
    else:
        attr = 1 << 4
        rnd = random.randint(1, 0xFF)
        body_payload = xor_crypt(payload, rnd, key)

    cs2 = checksum16(payload)                                # 明文校验
    body = struct.pack("<HBBHH", sn, cmd, rnd, cs2, len(payload))
    cs1 = (checksum16(bytes([attr])) + checksum16(body + body_payload)) & 0xFFFF
    return bytes([MARK, attr]) + struct.pack("<H", cs1) + body + body_payload


def read_frame(ser, timeout, key=None, state=None):
    """返回 (cmd, 明文 payload, sn, rnd)；超时返回 None。
    state 可选：dict，解密后若发现设备用密钥帧 0x70/0xF1 广播了新 K_L，
    则写 state['key'] = new_kL，供上层迭代更新会话密钥。"""
    deadline = time.time() + timeout
    stray = bytearray()

    def flush():
        global LAST_VERSION
        if not stray:
            return
        for line in stray.decode("utf-8", errors="replace").splitlines():
            line = line.strip()
            if len(line) < 2:
                continue
            # 门锁日志里带版本号，压测要靠它判断版本有没有翻转，
            # 所以即使不显示也要先解析出来
            m = VERSION_RE.search(line)
            if m:
                LAST_VERSION = int(m.group(1))
            if SHOW_DEV_LOG:
                log(f"  [dev] {line}")
        stray.clear()

    while time.time() < deadline:
        b = ser.read(1)
        if not b:
            continue
        if b[0] != MARK:
            stray.append(b[0])
            continue

        rest = ser.read(HEAD_SIZE - 1)
        if len(rest) != HEAD_SIZE - 1:
            continue

        attr = rest[0]
        cs1 = struct.unpack_from("<H", rest, 1)[0]
        sn = struct.unpack_from("<H", rest, 3)[0]
        cmd = rest[5]
        rnd = rest[6]
        cs2 = struct.unpack_from("<H", rest, 7)[0]
        length = struct.unpack_from("<H", rest, 9)[0]
        enc = (attr >> 4) & 0x03

        payload = b""
        while len(payload) < length:
            chunk = ser.read(length - len(payload))
            if not chunk:
                break
            payload += chunk
        if len(payload) != length:
            continue

        # checksum1 覆盖密文
        calc1 = (checksum16(bytes([attr])) + checksum16(rest[3:] + payload)) & 0xFFFF
        if cs1 != calc1:
            continue

        # 解密：密钥交换帧无论是否已有 K_L 都用 K_F，其余加密帧用当前会话密钥
        plain = payload
        if enc != 0:
            use = _decrypt_key_for_cmd(cmd, key)
            if use is None:
                continue                       # 还没协商到密钥，先丢弃
            plain = xor_crypt(payload, rnd, use)

        # checksum2 覆盖明文
        if checksum16(plain) != cs2:
            continue

        # 解密成功：若是 0x70/0xF1 密钥帧，提取新 K_L 写进 state
        if state is not None and cmd in (CMD_KEY_SEND, ACK_KEY_REQ) and len(plain) >= 4:
            new_kl = plain[KEY_INDEX]
            cur_kl = state.get('key', key)
            if 0x00 < new_kl < 0xFF and new_kl != cur_kl:
                state['key'] = new_kl
                if TRACE:
                    log(f"  [key] 发现新会话密钥 K_L = 0x{new_kl:02X}（来自 cmd=0x{cmd:02X}）")

        flush()
        if TRACE:
            log(f"  RX cmd=0x{cmd:02X} sn={sn} len={len(plain)}")
        return cmd, plain, sn, rnd

    flush()
    return None


def wait_for(ser, expect, timeout, key=None, also=None, state=None):
    """等待 expect（或 also）命令到来。
    state 可选；若期间发现 0x70/0xF1 广播了新 K_L，会自动更新 state['key']，
    后续所有收发都用新密钥。返回 (cmd, payload)；超时返回 None。"""
    deadline = time.time() + timeout
    while time.time() < deadline:
        effective_key = state['key'] if state else key
        f = read_frame(ser, max(0.05, deadline - time.time()), effective_key, state)
        if f is None:
            break
        cmd, payload, sn, rnd = f
        if cmd == expect or (also is not None and cmd == also):
            # ACK 匹配到的非 ACK 帧（含 0x74），避免对端重传耗尽重试次数
            if not (cmd & 0x80):
                use = _decrypt_key_for_cmd(cmd, effective_key)
                send_frame(ser, cmd | 0x80, b"", sn, use)
                ser.flush()
            return cmd, payload
        # 设备的其它请求（心跳 0x67、密钥帧 0x70 等）回 ACK，避免它重传到超时
        if not (cmd & 0x80):
            use = _decrypt_key_for_cmd(cmd, effective_key)
            send_frame(ser, cmd | 0x80, b"", sn, use)
            ser.flush()
    return None


def negotiate_key(ser):
    """发 0x71（K_F 加密）请求密钥；设备也可能主动发 0x70。
    期间要回应设备的其它请求（心跳 0x67 等），否则它会一直重传到超时。
    返回 (K_L, sn)，失败返回 (None, sn)。"""
    sn = 1
    for _ in range(3):
        send_frame(ser, CMD_KEY_REQ, b"", sn, KEY_FACTORY)
        ser.flush()

        deadline = time.time() + 3.0
        while time.time() < deadline:
            f = read_frame(ser, max(0.05, deadline - time.time()), KEY_FACTORY)
            if f is None:
                break
            cmd, payload, dev_sn, rnd = f

            # 密钥帧：0xF1 是 0x71 的应答，0x70 是设备主动广播
            if cmd in (ACK_KEY_REQ, CMD_KEY_SEND) and len(payload) >= 4:
                kl = payload[KEY_INDEX]
                if cmd == CMD_KEY_SEND:
                    send_frame(ser, ACK_KEY_SEND, b"", dev_sn, KEY_FACTORY)
                    ser.flush()
                if kl not in (0x00, 0xFF):
                    return kl, sn
                continue

            # 设备的其它请求（心跳等）→ 回 ACK，避免它重传到超时
            if not (cmd & 0x80):
                send_frame(ser, cmd | 0x80, b"", dev_sn, KEY_FACTORY)
                ser.flush()
        sn += 1
    return None, sn


def run_upload(port, fw_path, baud=230400, pkt=OTA_PKT_DEFAULT,
               retry=3, log_dir=None,
               dev_log=False, trace=True, quiet=False, sink=None):
    """执行一次 OTA 升级。
    成功返回 None，设备版本相同返回 OTA_SKIPPED，失败抛 OtaError。
    sink 是每条日志的回调，GUI 用它刷新界面。"""
    global SHOW_DEV_LOG, TRACE, LOG_FP, LOG_SINK, LAST_VERSION
    SHOW_DEV_LOG = dev_log
    TRACE = trace
    LOG_SINK = sink
    LAST_VERSION = None
    t_start = time.time()

    try:
        import serial
    except ImportError:
        die("缺少 pyserial，请执行 pip install pyserial")

    open_log(log_dir)

    with open(fw_path, "rb") as fp:
        fw = fp.read()
    if len(fw) < OTA_HEADER_SIZE:
        die("固件太小")

    magic = struct.unpack_from("<H", fw, 2)[0]
    if magic != OTA_MAGIC:
        die(f"文件头 magic=0x{magic:04X}，不是 0x{OTA_MAGIC:04X}，需用 packer 生成带头部的 bin")

    hdr_size = struct.unpack_from("<I", fw, 8)[0]
    version = fw[5:8]

    # 0x72 的 size 必须等于"实际发送的字节数"（收尾时设备比对 expected == total），
    # 而 OTA 区是从 16 字节文件头开始存的（boot 要在偏移 0 读到 0xBEEF），
    # 设备按 ota_area_addr() + expected 写入且 expected 从 0 起算，
    # 所以整个 .bin（含头）都要发；但 checksum 只算数据段——
    # 镜像头里的 checksum2 就是数据段的累加和，固件按同口径比对。
    fw_size = len(fw)
    fw_csum = checksum32(fw[OTA_HEADER_SIZE:])

    log(f"文件   : {fw_path}")
    log(f"长度   : {len(fw)} 字节（头部 size 字段 = {hdr_size}，发送 {fw_size} 字节含头）")
    log(f"版本   : {version[0]}.{version[1]}.{version[2]}")
    log(f"校验和 : 0x{fw_csum:08X}  （仅数据段，不含 16 字节头）")
    log(f"串口   : {port} @ {baud}, 每包 {pkt} 字节")
    log("-" * 56)

    try:
        ser = serial.Serial(port, baud, timeout=0.05)
    except serial.SerialException as exc:
        die(f"串口打开失败: {port}（{exc}）")
    ser.reset_input_buffer()

    # ---- 0. 密钥协商 ----
    log("密钥协商中...")
    key, sn = negotiate_key(ser)
    if key is None:
        die("密钥协商失败：没收到 0xF1/0x70 密钥帧")
    log(f"  门锁密钥 K_L = 0x{key:02X}")
    sn += 1

    # 用可变 state 保存会话密钥；wait_for/read_frame 收到 0x70/0xF1 广播时
    # 会自动更新 state['key']，这样设备重启后无需重新走 0x71 协商
    state = {'key': key}

    try:
        # ---- 1. 0x72 开始（设备擦 64K，等久一点）----
        payload = struct.pack("<I", fw_size) + struct.pack("<I", fw_csum) + bytes(version)
        r = None
        for attempt in range(retry):
            send_frame(ser, CMD_OTA_START, payload, sn, state['key'])
            ser.flush()
            r = wait_for(ser, ACK_START, 10.0, state=state)
            if r:
                break
            log(f"  0x72 无应答，重试 {attempt + 1}/{retry}")
        if not r:
            die("升级开始无应答（检查串口/波特率/设备是否在跑）")

        status = r[1][0]
        pkt = struct.unpack_from("<H", r[1], 1)[0] if len(r[1]) >= 3 else 0
        if status == 3:
            log("设备版本与镜像相同，跳过升级")
            return OTA_SKIPPED
        if status == 4:
            die("OTA 区擦除失败（写保护/时序问题），查看设备日志里的 erase fail 行")
        if status == 2:
            die("设备还有未完成的升级（s_ota.active=1），断电重上电后重试")
        if status != 0:
            die(f"升级开始被拒绝，status={status}（1=参数/长度非法或 flash 不可用）")
        dev_pkt = pkt
        # 设备上报的 240 会撑爆它自己的接收队列，必须压到 239 以内
        if pkt > OTA_PKT_MAX:
            log(f"  包长 {pkt} 超过接收队列上限，压到 {OTA_PKT_MAX}")
            pkt = OTA_PKT_MAX
        # 再向下取整到 4 的倍数：设备端的校验和是 4 字节小端累加，
        # 每包各自对齐时包长不是 4 的倍数会与整块连续累加不一致。
        # 新固件已做跨包对齐，但老固件没有，这里对齐可兼容两者。
        pkt = (pkt // 4) * 4
        log(f"设备就绪，建议包长 {dev_pkt}，实际使用 {pkt}")
        sn += 1

        # ---- 2. 0x73 数据 ----
        offset = 0
        last_pct = -1
        while offset < fw_size:
            n = min(pkt, fw_size - offset)
            # 从文件偏移 0 开始发，包含 16 字节头
            body = struct.pack("<I", offset) + fw[offset:offset + n]
            r = None
            for attempt in range(retry):
                send_frame(ser, CMD_OTA_DATA, body, sn, state['key'])
                ser.flush()
                r = wait_for(ser, ACK_DATA, 5.0, state=state)
                if r:
                    break
            if not r:
                die(f"数据包无应答，停在 0x{offset:X}")

            st = r[1][0]
            dev_off = struct.unpack_from("<I", r[1], 1)[0] if len(r[1]) >= 5 else 0
            if st == 0:
                offset = dev_off if dev_off else offset + n
            elif st == 1:
                offset = dev_off
            else:
                die(f"数据被拒绝，status={st}（2=写失败 3=回读失败 4=无升级会话），停在 0x{offset:X}")

            sn = (sn + 1) & 0xFFFF
            pct = offset * 100 // fw_size
            if pct // 10 != last_pct // 10:
                last_pct = pct
                if not quiet:
                    log(f"  进度 {pct:3d}%  ({offset}/{fw_size})")

        # ---- 3. 结束包 ----
        r = None
        for attempt in range(retry):
            send_frame(ser, CMD_OTA_DATA, struct.pack("<I", fw_size), sn, state['key'])
            ser.flush()
            r = wait_for(ser, ACK_DATA, 5.0, state=state, also=CMD_OTA_RESULT)
            if r:
                break
            log(f"  结束包无应答，重试 {attempt + 1}/{retry}")
        if not r:
            die("结束包没有应答")

        if r[0] == CMD_OTA_RESULT:
            res = r                       # 兼容：设备直接回了结果帧
        else:
            # 结束包的 ACK 只表示「数据收齐并通过校验」，不代表 OTA 完成
            st = r[1][0]
            if st != 0:
                reasons = {1: "偏移不连续", 2: "写 flash 失败", 3: "回读校验失败"}
                die(f"结束校验失败：{reasons.get(st, st)}")

            # 0x74 结果帧现在由「boot 把镜像搬到 FMC、跳回 app」之后上报，
            # 中间隔着一次重启（约 10 秒），要给足超时。
            # 重启后设备会用新的 K_L 广播 0x70，read_frame 会自动用 K_F 解出并更新 state。
            log("下载完成，等待 boot 搬运并回到 app 后上报结果...")
            res = wait_for(ser, CMD_OTA_RESULT, 40.0, state=state)
            if res is None:
                die("没收到 0x74 结果（boot 搬运或重启失败）；"
                    "若期间看到 [key] 打印说明密钥已更新但 0x74 仍没来")

        result = res[1][0]
        ver = res[1][1:4]
        if result != 0:
            reasons = {1: "校验和/回读/搬运失败", 2: "长度不符"}
            die(f"{reasons.get(result, f'result={result}')}")
        # 设备上报的是本机真实版本（sn 拆分 + PATCH_VERSION），
        # 例 39.140.005，可用最后一段核对版本是否真的翻转
        log(f"OTA 完成：成功，新固件版本 V{ver[0]}.{ver[1]}.{ver[2]:03d}，"
            f"总耗时 {time.time() - t_start:.1f} 秒")

        # 核对版本：0x74 是 boot 搬运完成后由新固件发出的，
        # 所以它报的 PATCH 必须等于目标文件的 PATCH，否则说明搬的是旧镜像
        want = expected_patch(fw_path)
        if (want is not None) and (ver[2] != want):
            die(f"设备仍在跑 V{ver[0]}.{ver[1]}.{ver[2]:03d}，"
                f"目标 PATCH={want:03d}（boot 搬运未生效）")

        # 0x74 成功即代表 boot 已搬运完成、app 已跑新固件，流程结束，不再监听

    finally:
        try:
            ser.close()
        except Exception:
            pass

    if LAST_VERSION is not None:
        log(f"最终版本: V39.140.{LAST_VERSION:03d}")
    else:
        log("最终版本: 未知")
    return None
