#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Keil 编译后生成两份固件（复制改名，不做任何切割）

    python application\\Project\\make_fw.py

输入（packer.exe 的产物）：
    application\\Project\\combine_0.0.1.hex          -> 正式固件
    application\\Build\\Objects\\ota_app.hex.bin     -> OTA 固件

输出（本目录下的 firmware\\）：
    firmware\\release\\V39_140_XXX.hex   正式固件，SWD/烧录器整片烧写
    firmware\\ota\\V39_140_XXX.bin       OTA 固件，串口升级用

文件名 = CONFIG_SEL + PROJECT_NUMBER + PATCH_VERSION
    CONFIG_SEL / PROJECT_NUMBER ：ota_app.uvproj 的 <Define>
    PATCH_VERSION               ：item_39140.h
"""

import io
import os
import re
import shutil
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


def main():
    ver = read_patch_version()
    sel, cfg = read_model_ids()
    if ver is None or sel is None or cfg is None:
        log(f"解析失败: PATCH_VERSION={ver} CONFIG_SEL={sel} PROJECT_NUMBER={cfg}")
        return 1

    name = f"V{sel}_{cfg}_{ver:03d}"

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

    shutil.copyfile(SRC_HEX, hex_dst)
    shutil.copyfile(SRC_BIN, bin_dst)

    log(f"版本 {name}")
    log(f"  -> {os.path.relpath(hex_dst, HERE)}   正式固件 {os.path.getsize(hex_dst)} 字节")
    log(f"  -> {os.path.relpath(bin_dst, HERE)}   OTA 固件  {os.path.getsize(bin_dst)} 字节")
    return 0


if __name__ == "__main__":
    sys.exit(main())
