#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
由 Keil 工程（application/Project/ota_app.uvproj）生成 IDE 智能感知配置：

    .clangd                        CodeBuddy / clangd 使用（-I 用绝对路径）
    .vscode/c_cpp_properties.json  VSCode C/C++ 扩展使用（${workspaceFolder} 相对路径）

两份配置的 includePath / defines 都从 uvproj 的 IncludePath、Define 转换而来，
另补上 Keil ARM 工具链的标准库头目录（string.h / stdint.h 等）。

调用方式：
    1. Keil 编译时自动执行（已挂到 Target Options -> User -> Before Make）
    2. 手动执行：python tools/gen_ide_config.py

环境变量：
    KEIL_ARM   Keil ARM 目录，默认 C:/Keil_v5/ARM
"""

import glob
import io
import json
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__))).replace('\\', '/')
UVPROJ = ROOT + '/application/Project/ota_app.uvproj'

KEIL_ARM = os.environ.get('KEIL_ARM', 'C:/Keil_v5/ARM').replace('\\', '/')
ARMCLANG_ROOT = KEIL_ARM + '/ARMCLANG'

CCPP_PATH = ROOT + '/.vscode/c_cpp_properties.json'
CLANGD_PATH = ROOT + '/.clangd'

CLANGD_HEADER = [
    '# 本文件由 tools/gen_ide_config.py 自动生成，请勿手工修改',
    '# 数据来源：application/Project/ota_app.uvproj 的 IncludePath / Define',
]


def read_text(path):
    with io.open(path, encoding='utf-8', errors='ignore') as f:
        return f.read()


def longest_tag(text, tag):
    found = re.findall(r'<%s>(.*?)</%s>' % (tag, tag), text, re.S)
    found = [v for v in found if v.strip()]
    return max(found, key=len) if found else ''


def parse_cpu(text):
    """从 <Cpu>...CPUTYPE("Cortex-M0")...</Cpu> 提取 -mcpu 取值"""
    m = re.search(r'CPUTYPE\("([^"]+)"\)', text)
    return m.group(1).strip().lower() if m else ''


def toolchain_includes():
    """Keil ARM 工具链的标准库头目录（不存在的自动跳过）"""
    dirs = []
    for d in (ARMCLANG_ROOT + '/include',):
        if os.path.isdir(d):
            dirs.append(d)
    for d in sorted(glob.glob(ARMCLANG_ROOT + '/lib/clang/*/include')):
        if os.path.isdir(d):
            dirs.append(d.replace('\\', '/'))
    return dirs


def dedup(seq):
    """按原顺序去重"""
    out = []
    for item in seq:
        if item not in out:
            out.append(item)
    return out


def keil_relative(path):
    """uvproj 里的 ..\\protocol 转为 相对 uvproj 目录的 -I 形式（正斜杠）"""
    return path.strip().replace('\\', '/')


def workspace_relative(path):
    """uvproj 里的 ..\\protocol 转为 相对工作区根目录的路径"""
    p = keil_relative(path)
    if not p:
        return None
    parts = p.split('/')
    n = 0
    while n < len(parts) and parts[n] == '..':
        n += 1
    base = ['application', 'Project']
    for _ in range(n):
        base.pop()
    rel = '/'.join([b for b in base if b] + parts[n:]).replace('/./', '/')
    return rel


def main():
    if not os.path.isfile(UVPROJ):
        sys.stderr.write('uvproj not found: %s\n' % UVPROJ)
        return 1

    text = read_text(UVPROJ)
    include_path = longest_tag(text, 'IncludePath')
    define = longest_tag(text, 'Define')
    cpu = parse_cpu(text)

    if not include_path:
        sys.stderr.write('no IncludePath found in uvproj\n')
        return 1

    defines = [d.strip() for d in define.split(',') if d.strip()]
    proj_dirs = dedup([d for d in (keil_relative(p) for p in include_path.split(';')) if d])
    extra_dirs = dedup(toolchain_includes())

    # ---------------- .clangd ----------------
    flags = ['--target=arm-arm-none-eabi']
    if cpu:
        flags.append('-mcpu=' + cpu)
    flags += ['-D' + d for d in defines]
    flags += ['-I' + (ROOT + '/' + workspace_relative(p)) for p in proj_dirs if workspace_relative(p)]
    flags += ['-I' + d for d in extra_dirs]

    lines = list(CLANGD_HEADER) + ['CompileFlags:', '  Add:']
    lines += ['    - "%s"' % f for f in flags]
    # -std 只能给 C 源文件：头文件语言由 clangd 推断（可能按 Objective-C++ 解析），
    # 公共片段里写 -std=gnu11 会报 "not allowed with 'Objective-C++'"
    lines += [
        '',
        '# 工程用 armcc --c99 --gnu 编译，标准只对 .c 源文件设置',
        '---',
        'If:',
        '  PathMatch: .*\\.c$',
        'CompileFlags:',
        '  Add:',
        '    - "-std=gnu99"',
    ]
    with io.open(CLANGD_PATH, 'w', encoding='utf-8', newline='\n') as f:
        f.write('\n'.join(lines) + '\n')

    # ---------------- c_cpp_properties.json ----------------
    ws_dirs = ['${workspaceFolder}/' + workspace_relative(p)
               for p in proj_dirs if workspace_relative(p)]
    cfg = {
        "version": 4,
        "configurations": [
            {
                "name": "Keil-MDK-ARMCLANG",
                "compilerPath": ARMCLANG_ROOT + '/bin/armclang.exe',
                "compilerArgs": ['--target=arm-arm-none-eabi'] + (['-mcpu=' + cpu] if cpu else []),
                "cStandard": "gnu99",
                "cppStandard": "gnu++14",
                "intelliSenseMode": "windows-clang-arm",
                "defines": defines,
                "includePath": ws_dirs + extra_dirs,
            }
        ],
    }
    if not os.path.isdir(os.path.dirname(CCPP_PATH)):
        os.makedirs(os.path.dirname(CCPP_PATH))
    with io.open(CCPP_PATH, 'w', encoding='utf-8', newline='\n') as f:
        f.write(json.dumps(cfg, indent=4, ensure_ascii=False) + '\n')

    print('gen_ide_config: %d include dirs, %d defines, cpu=%s'
          % (len(ws_dirs) + len(extra_dirs), len(defines), cpu or 'unknown'))
    print('  -> %s' % CLANGD_PATH.replace(ROOT + '/', ''))
    print('  -> %s' % CCPP_PATH.replace(ROOT + '/', ''))
    return 0


if __name__ == '__main__':
    sys.exit(main())
