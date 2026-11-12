#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
OTA 反复压测，统计通过率

    python tools\\ota_stress\\ota_stress.py --port COM7 --rounds 20

固件从本目录的 fw_a / fw_b 各取一个（PATCH 版本必须不同）：
    tools\\ota_stress\\fw_a\\V39_140_005.bin
    tools\\ota_stress\\fw_b\\V39_140_011.bin

每轮判定标准（三条同时满足才算通过）：
    1. ota_upload.py 退出码 0（下载 + 结束包校验通过）
    2. 监听窗口内收到 outside_version 日志
    3. 版本号等于目标固件的 PATCH_VERSION —— 这条最关键，
       只看「上传成功」会被骗：boot 搬运失败时也会报成功。

日志落在 tools\\ota_stress\\log\\。
"""

import argparse
import datetime
import os
import re
import subprocess
import sys
import time

HERE = os.path.dirname(os.path.abspath(__file__))          # tools\ota_stress
ROOT = os.path.dirname(os.path.dirname(HERE))              # 工程根目录
UPLOAD = os.path.join(ROOT, "tools", "ota_tool", "ota_upload.py")
FW_A_DIR = os.path.join(HERE, "fw_a")
FW_B_DIR = os.path.join(HERE, "fw_b")
LOG_DIR = os.path.join(HERE, "log")

VER_RE = re.compile(r"outside_version:\s*V39\.140\.(\d+)")


def log(msg):
    line = f"[{datetime.datetime.now().strftime('%H:%M:%S')}] {msg}"
    print(line, flush=True)


def expected_patch(path):
    m = re.search(r"_(\d{3})\.bin$", os.path.basename(path), re.IGNORECASE)
    return int(m.group(1)) if m else None


def pick_newest(d):
    if not os.path.isdir(d):
        return None
    files = [os.path.join(d, n) for n in os.listdir(d) if n.lower().endswith(".bin")]
    if not files:
        return None
    files.sort(key=os.path.getmtime, reverse=True)
    return files[0]


def detect_version(text):
    hits = VER_RE.findall(text)
    return int(hits[-1]) if hits else None


def run_round(port, fw, timeout, listen, quiet):
    cmd = [sys.executable, UPLOAD, "--port", port, "--file", fw,
           "--listen", str(listen), "--log-dir", LOG_DIR]
    if quiet:
        cmd.append("--quiet")

    t0 = time.time()
    try:
        p = subprocess.run(cmd, capture_output=True, timeout=timeout,
                           text=True, encoding="utf-8", errors="replace")
        out = (p.stdout or "") + (p.stderr or "")
        rc = p.returncode
    except subprocess.TimeoutExpired as e:
        out = ((e.stdout or b"").decode("utf-8", "replace")
               + (e.stderr or b"").decode("utf-8", "replace"))
        rc = -9
    return rc, out, time.time() - t0


def main():
    ap = argparse.ArgumentParser(description="OTA 反复压测")
    ap.add_argument("--port", required=True, help="串口，如 COM7")
    ap.add_argument("--rounds", type=int, default=20)
    ap.add_argument("--listen", type=float, default=18.0,
                    help="每轮收尾监听秒数，默认 18")
    ap.add_argument("--timeout", type=float, default=300.0,
                    help="单轮超时秒数，默认 300")
    ap.add_argument("--file-a", default=None)
    ap.add_argument("--file-b", default=None)
    ap.add_argument("--stop-on-fail", action="store_true")
    ap.add_argument("--quiet", action="store_true",
                    help="不打印 ota_upload 的逐包进度")
    args = ap.parse_args()

    fa = args.file_a or pick_newest(FW_A_DIR)
    fb = args.file_b or pick_newest(FW_B_DIR)
    if not fa or not fb or not os.path.isfile(fa) or not os.path.isfile(fb):
        log("压测需要两个不同版本的 .bin：")
        log(f"  固件 A -> {os.path.relpath(FW_A_DIR, ROOT)}")
        log(f"  固件 B -> {os.path.relpath(FW_B_DIR, ROOT)}")
        return 2

    va, vb = expected_patch(fa), expected_patch(fb)
    if va is None or vb is None:
        log(f"文件名必须形如 V39_140_005.bin: {fa} / {fb}")
        return 2
    if va == vb:
        log(f"两个固件的 PATCH 版本相同（都是 {va}），压测无意义")
        return 2

    os.makedirs(LOG_DIR, exist_ok=True)

    log("=" * 62)
    log(f"串口    : {args.port}")
    log(f"固件 A  : {os.path.basename(fa)}  -> 期望 V39.140.{va:03d}")
    log(f"固件 B  : {os.path.basename(fb)}  -> 期望 V39.140.{vb:03d}")
    log(f"轮数    : {args.rounds}")
    log(f"每轮监听: {args.listen:g}s    单轮超时: {args.timeout:g}s")
    log(f"失败策略: {'首败即停' if args.stop_on_fail else '继续跑完'}")
    log("=" * 62)

    cur_patch = None        # 设备当前版本（未知则 None）
    baseline_unknown = True  # 首轮只打底，不计入统计
    results = []
    started = time.time()

    for i in range(1, args.rounds + 1):
        # 挑一个与当前版本不同的固件，保证每轮都要翻转一次
        if cur_patch == va:
            fw, expect = fb, vb
        elif cur_patch == vb:
            fw, expect = fa, va
        else:
            fw, expect = (fb, vb) if (i % 2 == 1) else (fa, va)

        tag = f"[{i}/{args.rounds}] 目标 V39.140.{expect:03d}"
        log(f"{tag} 开始...")

        rc, out, dt = run_round(args.port, fw, args.timeout, args.listen, args.quiet)
        got_patch = detect_version(out)
        got_str = f"V39.140.{got_patch:03d}" if got_patch is not None else "未知"

        # 版本是否真的翻转是唯一可靠判据
        if got_patch is not None and got_patch == expect:
            ok, reason = True, ("" if rc == 0 else f"版本已翻转（脚本退出码 {rc}）")
        elif rc != 0:
            ok = False
            reason = "超时" if rc == -9 else "上传失败"
            m = re.search(r"^\[[^\]]+\]\s*(?:OTA 失败|结束校验失败|.*失败|.*错误)[^\n]*",
                          out, re.MULTILINE)
            if m:
                reason += "：" + m.group(0).split("]", 1)[-1].strip()[:60]
            if got_patch is not None:
                reason += f"；当前 {got_str}"
        elif got_patch is None:
            ok, reason = False, "未收到 outside_version（设备可能没起来）"
        else:
            ok, reason = False, f"版本未翻转，仍是 {got_str}"

        priming = baseline_unknown and ok
        if got_patch is not None:
            cur_patch = got_patch
        baseline_unknown = False

        results.append({"round": i, "expect": expect, "got": got_patch,
                        "got_str": got_str, "ok": ok, "priming": priming,
                        "reason": reason, "secs": dt})

        if priming:
            log(f"{tag} 打底  用时 {dt:5.1f}s  当前 {got_str}（基线未知，不计入统计）")
        elif ok:
            log(f"{tag} 通过  用时 {dt:5.1f}s  当前 {got_str}")
        else:
            log(f"{tag} 失败  用时 {dt:5.1f}s  当前 {got_str}  <- {reason}")
            if args.stop_on_fail:
                log("按 --stop-on-fail 中止")
                break

    # ---- 汇总 ----
    counted = [r for r in results if not r.get("priming")]
    total = len(counted)
    passed = sum(1 for r in counted if r["ok"])
    failed = total - passed
    elapsed = time.time() - started
    rate = (passed * 100.0 / total) if total else 0.0
    avg = (sum(r["secs"] for r in counted) / total) if total else 0.0

    log("=" * 62)
    log("压测汇总")
    log(f"  有效轮数  : {total}（另有 {len(results) - total} 轮打底，不计入）")
    log(f"  通过      : {passed}")
    log(f"  失败      : {failed}")
    log(f"  通过率    : {rate:.1f}%")
    log(f"  平均耗时  : {avg:.1f}s/轮")
    log(f"  总耗时    : {elapsed / 60.0:.1f} 分钟")

    if failed:
        log("  失败明细  :")
        for r in counted:
            if not r["ok"]:
                log(f"    第 {r['round']} 轮  目标 V39.140.{r['expect']:03d}"
                    f"  实际 {r['got_str']}  {r['reason']}")

    log("=" * 62)
    return 0 if failed == 0 else 1


if __name__ == "__main__":
    sys.exit(main())
