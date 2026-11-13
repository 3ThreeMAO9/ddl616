#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Keil 编译后生成两份固件，并把镜像头版本改成真实版本号

    python application\\Project\\make_fw.py

输入（packer.exe 的产物）：
    application\\Project\\combine_0.0.1.hex          -> 正式固件
    application\\Build\\Objects\\ota_app.hex.bin     -> OTA 固件

输出（本目录下的 firmware\\）：
    firmware\\release\\V39_140_XXX.hex   正式固件，SWD/烧录器整片烧写
    firmware\\ota\\V39_140_XXX.bin       OTA 固件，串口升级用

镜像头 16 字节里的 version 由 packer 的 -v 决定（固定 0.0.1），这里统一改成
真实版本 [CONFIG_SEL][PROJECT_NUMBER][PATCH_VERSION]，即 39.140.005，
并重算覆盖 version 的 checksum1，保证 boot/app 的头部校验仍然通过。

文件名 = CONFIG_SEL + PROJECT_NUMBER + PATCH_VERSION
    CONFIG_SEL / PROJECT_NUMBER ：ota_app.uvproj 的 <Define>
    PATCH_VERSION               ：item_39140.h
"""

import io
import os
import re
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))          # application\Project
ROOT = os.path.dirname(os.path.dirname(HERE))              # 工程根目录

SRC_HEX = os.path.join(HERE, "combine_0.0.1.hex")
SRC_BIN = os.path.join(HERE, "..", "Build", "Objects", "ota_app.hex.bin")

ITEM_H = os.path.join(ROOT, "application", "config", "item", "item_info", "item_39140.h")
UVPROJ = os.path.join(HERE, "ota_app.uvproj")

FW_DIR = os.path.join(HERE, "firmware")
OTA_DIR = os.path.join(FW_DIR, "ota")
REL_DIR = os.path.join(FW_DIR, "release")

HDR_ADDR = 0x400        # app 镜像头在 FMC 中的地址
HDR_SIZE = 16


def log(msg=""):
    sys.stdout.write(f"[make_fw] {msg}\n")
    sys.stdout.flush()


def read_patch_version():
    with io.open(ITEM_H, encoding="utf-8", errors="replace") as fp:
        m = re.search(r"#define\s+PATCH_VERSION\s+(\d+)", fp.read())
    return int(m.group(1)) if m else None


def read_model_ids():
    """从 uvproj 的 <Define> 取 CONFIG_SEL / PROJECT_NUMBER"""
    sel = cfg = None
    with io.open(UVPROJ, encoding="utf-8", errors="replace") as fp:
        for line in fp:
            if "<Define>" not in line:
                continue
            m = re.search(r"PROJECT_NUMBER\s*=\s*(\d+)", line)
            if m:
                cfg = int(m.group(1))
            m = re.search(r"CONFIG_SEL\s*=\s*(\d+)", line)
            if m:
                sel = int(m.group(1))
            if sel is not None and cfg is not None:
                break
    return sel, cfg


def make_header(buf, ver3):
    """把 16 字节镜像头的版本换成 ver3，并重算 checksum1（覆盖 2~15 字节）"""
    hdr = bytearray(buf[:HDR_SIZE])
    hdr[5:8] = ver3
    struct.pack_into("<H", hdr, 0,
                     sum(struct.unpack_from("<7H", hdr, 2)) & 0xFFFF)
    return bytes(hdr)


def _hex_parse(line):
    return bytearray.fromhex(line[1:])


def hex_read(text, start, size):
    """从 Intel HEX 里读出 [start, start+size) 的内容"""
    got = {}
    upper = 0
    for line in text.splitlines():
        line = line.strip()
        if len(line) < 11 or line[0] != ":":
            continue
        raw = _hex_parse(line)
        cnt, addr, typ = raw[0], (raw[1] << 8) | raw[2], raw[3]
        if typ == 4:
            upper = (raw[4] << 8) | raw[5]
        elif typ == 0:
            base = (upper << 16) + addr
            for i in range(cnt):
                a = base + i
                if start <= a < start + size:
                    got[a - start] = raw[4 + i]

    if len(got) != size:
        raise RuntimeError(f"HEX 中 0x{start:04X} 处缺少数据")
    return bytes(got[i] for i in range(size))


def hex_write(text, start, data):
    """把 data 写到 HEX 的 start 地址，并重算被修改记录的校验"""
    out = []
    upper = 0
    written = 0
    for line in text.splitlines():
        line = line.strip()
        if len(line) < 11 or line[0] != ":":
            continue
        raw = _hex_parse(line)
        cnt, addr, typ = raw[0], (raw[1] << 8) | raw[2], raw[3]
        if typ == 4:
            upper = (raw[4] << 8) | raw[5]
        elif typ == 0:
            base = (upper << 16) + addr
            touched = False
            for i in range(cnt):
                a = base + i
                if start <= a < start + len(data):
                    raw[4 + i] = data[a - start]
                    written += 1
                    touched = True
            if touched:
                raw[-1] = (-sum(raw[:-1])) & 0xFF
        out.append(":" + bytes(raw).hex().upper())

    if written != len(data):
        raise RuntimeError(f"HEX 中 0x{start:04X} 处写入失败")
    return "\n".join(out) + "\n"


def main():
    ver = read_patch_version()
    sel, cfg = read_model_ids()
    if ver is None or sel is None or cfg is None:
        log(f"解析失败: PATCH_VERSION={ver} CONFIG_SEL={sel} PROJECT_NUMBER={cfg}")
        return 1

    name = f"V{sel}_{cfg}_{ver:03d}"

    # 与 0x74 上报、uart_msg_front_version_str() 一致的 3 字节版本：
    # [CONFIG_SEL][PROJECT_NUMBER][PATCH_VERSION]，即 39.140.005
    ver3 = bytes([sel, cfg, ver])
    if any(b > 255 for b in ver3):
        log(f"版本 {ver3} 超出单字节范围")
        return 1

    if not os.path.isfile(SRC_HEX):
        log(f"缺少 {os.path.basename(SRC_HEX)}，检查 After Build 的 packer.exe 是否执行成功")
        return 1
    if not os.path.isfile(SRC_BIN):
        log(f"缺少 {os.path.relpath(SRC_BIN, HERE)}，packer.exe 会同时生成它")
        return 1

    os.makedirs(OTA_DIR, exist_ok=True)
    os.makedirs(REL_DIR, exist_ok=True)

    hex_dst = os.path.join(REL_DIR, name + ".hex")
    bin_dst = os.path.join(OTA_DIR, name + ".bin")

    # 1. OTA bin：直接改文件头
    with open(SRC_BIN, "rb") as fp:
        bin_data = fp.read()
    if len(bin_data) < HDR_SIZE:
        log("OTA bin 长度异常")
        return 1
    with open(bin_dst, "wb") as fp:
        fp.write(make_header(bin_data, ver3) + bin_data[HDR_SIZE:])

    # 2. 正式 hex：改 app 镜像头所在记录（校验和随之重算）
    with io.open(SRC_HEX, encoding="ascii", errors="replace") as fp:
        hex_text = fp.read()
    old_hdr = hex_read(hex_text, HDR_ADDR, HDR_SIZE)
    hex_text = hex_write(hex_text, HDR_ADDR, make_header(old_hdr, ver3))
    with io.open(hex_dst, "w", encoding="ascii", newline="\n") as fp:
        fp.write(hex_text)

    log(f"版本 {name}  镜像头 {ver3[0]}.{ver3[1]}.{ver3[2]}")
    log(f"  -> {os.path.relpath(hex_dst, HERE)}   正式固件 {os.path.getsize(hex_dst)} 字节")
    log(f"  -> {os.path.relpath(bin_dst, HERE)}   OTA 固件  {os.path.getsize(bin_dst)} 字节")
    return 0


if __name__ == "__main__":
    sys.exit(main())
