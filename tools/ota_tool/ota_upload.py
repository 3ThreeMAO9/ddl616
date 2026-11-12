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

import argparse
import datetime
import os
import random
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

OTA_MAGIC = 0xBEEF
OTA_HEADER_SIZE = 16

# 设备在 0xF2 里上报的是 protocol.h 的 OTA_PKT_SIZE = 240，
# 但门锁的接收队列 UART_RX_QUEUE_SIZE = 256（环形缓冲可用 255），
# 一帧 = 12(头) + 4(offset) + pkt，pkt=240 时正好 256 字节，最后一字节存不下，
# 整帧会被丢弃 —— 表现为设备只回了 0xF2 就再无响应。
# 所以实际必须 <= 239，默认取 224 留足余量。
OTA_PKT_SIZE = 240          # 设备上报值，仅供参考
OTA_PKT_MAX = 239           # 受接收队列限制的实际上限
OTA_PKT_DEFAULT = 224       # 实际使用的包长

HERE = os.path.dirname(os.path.abspath(__file__))

LOG_FP = None


def log(msg=""):
    ts = datetime.datetime.now().strftime("%H:%M:%S.%f")[:-3]
    line = f"[{ts}] {msg}"
    sys.stdout.write(line + "\n")
    sys.stdout.flush()
    if LOG_FP:
        LOG_FP.write(line + "\n")
        LOG_FP.flush()


def die(msg):
    log(f"OTA 失败：{msg}")
    sys.exit(1)


def open_log(log_dir=None):
    """日志目录可指定（压测脚本会用 ota_stress\\log），默认 ota_tool\\log"""
    global LOG_FP
    d = log_dir or os.path.join(HERE, "log")
    os.makedirs(d, exist_ok=True)
    name = datetime.datetime.now().strftime("ota_%Y%m%d_%H%M%S.log")
    LOG_FP = open(os.path.join(d, name), "w", encoding="utf-8")


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


def read_frame(ser, timeout, key=None):
    """返回 (cmd, 明文 payload, sn)；超时返回 None。
    非协议字节按行以 [dev] 前缀打印，方便看设备日志。"""
    deadline = time.time() + timeout
    stray = bytearray()

    def flush():
        if not stray:
            return
        for line in stray.decode("utf-8", errors="replace").splitlines():
            line = line.strip()
            if len(line) >= 2:
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

        # 解密：密钥交换帧用 K_F，其余用 K_L
        plain = payload
        if enc != 0:
            use = key
            if use is None and (cmd & 0x7F) == CMD_KEY_SEND:
                use = KEY_FACTORY
            if use is None:
                continue                       # 还没协商到密钥，先丢弃
            plain = xor_crypt(payload, rnd, use)

        # checksum2 覆盖明文
        if checksum16(plain) != cs2:
            continue

        flush()
        return cmd, plain, sn, rnd

    flush()
    return None


def wait_for(ser, expect, timeout, key=None, also=None):
    deadline = time.time() + timeout
    while time.time() < deadline:
        f = read_frame(ser, max(0.05, deadline - time.time()), key)
        if f is None:
            break
        cmd, payload, sn, rnd = f
        if cmd == expect or (also is not None and cmd == also):
            return cmd, payload
        # 设备的请求用 K_F 相关的回 ACK，其余用当前会话密钥
        if not (cmd & 0x80):
            use = KEY_FACTORY if (cmd & 0x7F) == CMD_KEY_SEND else key
            ser.write(build_frame(cmd | 0x80, b"", sn, use))
            ser.flush()
    return None


def negotiate_key(ser):
    """发 0x71（K_F 加密）请求密钥；设备也可能主动发 0x70。
    期间要回应设备的其它请求（心跳 0x67 等），否则它会一直重传到超时。
    返回 (K_L, sn)，失败返回 (None, sn)。"""
    sn = 1
    for _ in range(3):
        ser.write(build_frame(CMD_KEY_REQ, b"", sn, KEY_FACTORY))
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
                    ser.write(build_frame(ACK_KEY_SEND, b"", dev_sn, KEY_FACTORY))
                    ser.flush()
                if kl not in (0x00, 0xFF):
                    return kl, sn
                continue

            # 设备的其它请求（心跳等）→ 回 ACK，避免它重传到超时
            if not (cmd & 0x80):
                ser.write(build_frame(cmd | 0x80, b"", dev_sn, KEY_FACTORY))
                ser.flush()
        sn += 1
    return None, sn


def main():
    ap = argparse.ArgumentParser(description="串口 OTA 上传（加密）")
    ap.add_argument("--port", required=True, help="串口，如 COM7")
    ap.add_argument("--file", required=True, help="带 16 字节头的 OTA bin")
    ap.add_argument("--baud", type=int, default=230400)
    ap.add_argument("--pkt", type=int, default=OTA_PKT_DEFAULT,
                    help=f"单包数据长度，默认 {OTA_PKT_DEFAULT}，上限 {OTA_PKT_MAX}")
    ap.add_argument("--retry", type=int, default=3)
    ap.add_argument("--listen", type=float, default=30.0)
    ap.add_argument("--quiet", action="store_true")
    ap.add_argument("--log-dir", default=None,
                    help="日志输出目录，默认 ota_tool\\log")
    args = ap.parse_args()

    try:
        import serial
    except ImportError:
        die("缺少 pyserial，请执行 pip install pyserial")

    open_log(args.log_dir)

    with open(args.file, "rb") as fp:
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

    log(f"文件   : {args.file}")
    log(f"长度   : {len(fw)} 字节（头部 size 字段 = {hdr_size}，发送 {fw_size} 字节含头）")
    log(f"版本   : {version[0]}.{version[1]}.{version[2]}")
    log(f"校验和 : 0x{fw_csum:08X}  （仅数据段，不含 16 字节头）")
    log(f"串口   : {args.port} @ {args.baud}, 每包 {args.pkt} 字节")
    log("-" * 56)

    try:
        ser = serial.Serial(args.port, args.baud, timeout=0.05)
    except serial.SerialException as exc:
        die(f"串口打开失败: {args.port}（{exc}）")
    ser.reset_input_buffer()

    # ---- 0. 密钥协商 ----
    log("密钥协商中...")
    key, sn = negotiate_key(ser)
    if key is None:
        die("密钥协商失败：没收到 0xF1/0x70 密钥帧")
    log(f"  门锁密钥 K_L = 0x{key:02X}")
    sn += 1

    try:
        # ---- 1. 0x72 开始（设备擦 64K，等久一点）----
        payload = struct.pack("<I", fw_size) + struct.pack("<I", fw_csum) + bytes(version)
        r = None
        for attempt in range(args.retry):
            ser.write(build_frame(CMD_OTA_START, payload, sn, key))
            ser.flush()
            r = wait_for(ser, ACK_START, 10.0, key)
            if r:
                break
            log(f"  0x72 无应答，重试 {attempt + 1}/{args.retry}")
        if not r:
            die("升级开始无应答（检查串口/波特率/设备是否在跑）")

        status = r[1][0]
        pkt = struct.unpack_from("<H", r[1], 1)[0] if len(r[1]) >= 3 else 0
        if status == 2:
            die("status=2 有两种含义：\n"
                "       a) 设备还停留在上一次未完成的升级（s_ota.active=1）\n"
                "       b) OTA 区擦除失败（写保护/时序问题），固件里两种都回 2\n"
                "       a) 断电重上电后重试；b) 看设备日志里的 sr / write fail 行")
        if status != 0:
            die(f"升级开始被拒绝，status={status}（1=长度超限/flash 不可用）")
        if pkt:
            args.pkt = min(args.pkt, pkt)
        # 设备上报的 240 会撑爆它自己的接收队列，必须压到 239 以内
        if args.pkt > OTA_PKT_MAX:
            log(f"  包长 {args.pkt} 超过接收队列上限，压到 {OTA_PKT_MAX}")
            args.pkt = OTA_PKT_MAX
        log(f"设备就绪，建议包长 {pkt}，实际使用 {args.pkt}")
        sn += 1

        # ---- 2. 0x73 数据 ----
        offset = 0
        last_pct = -1
        while offset < fw_size:
            n = min(args.pkt, fw_size - offset)
            # 从文件偏移 0 开始发，包含 16 字节头
            body = struct.pack("<I", offset) + fw[offset:offset + n]
            r = None
            for attempt in range(args.retry):
                ser.write(build_frame(CMD_OTA_DATA, body, sn, key))
                ser.flush()
                r = wait_for(ser, ACK_DATA, 5.0, key)
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
                die(f"写 flash 失败，status={st}，停在 0x{offset:X}")

            sn = (sn + 1) & 0xFFFF
            pct = offset * 100 // fw_size
            if pct // 10 != last_pct // 10:
                last_pct = pct
                if not args.quiet:
                    log(f"  进度 {pct:3d}%  ({offset}/{fw_size})")

        # ---- 3. 结束包 ----
        r = None
        for attempt in range(args.retry):
            ser.write(build_frame(CMD_OTA_DATA, struct.pack("<I", fw_size), sn, key))
            ser.flush()
            r = wait_for(ser, ACK_DATA, 5.0, key, also=CMD_OTA_RESULT)
            if r:
                break
            log(f"  结束包无应答，重试 {attempt + 1}/{args.retry}")
        if not r:
            die("结束包没有应答")

        if r[0] == CMD_OTA_RESULT:
            res = r
        else:
            st = r[1][0]
            if st != 0:
                reasons = {1: "偏移不连续", 2: "写 flash 失败"}
                die(f"结束校验失败：{reasons.get(st, st)}")
            res = wait_for(ser, CMD_OTA_RESULT, 8.0, key)
            if res is None:
                die("没收到 0x74 结果")

        result = res[1][0]
        ver = res[1][1:4]
        if result != 0:
            reasons = {1: "校验和错误", 2: "长度不符"}
            die(f"{reasons.get(result, f'result={result}')}")
        log(f"OTA 完成：成功，新固件版本 {ver[0]}.{ver[1]}.{ver[2]}")

        # ---- 4. 监听重启与版本 ----
        log(f"等待设备重启（跳 boot 搬运，继续监听 {args.listen:g} 秒）...")
        deadline = time.time() + args.listen
        while time.time() < deadline:
            read_frame(ser, 0.3, key)

    finally:
        try:
            ser.close()
        except Exception:
            pass

    log("结束")
    return 0


if __name__ == "__main__":
    sys.exit(main())
