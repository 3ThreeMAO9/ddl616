#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
OTA 上位机

    python tools\\ota_host\\ota_gui.py

功能：
    * 选择串口（自动枚举 COM）
    * 选择固件 A / 固件 B
    * 单次升级：选 A 或 B 升级一次
    * 循环升级：反复在 A / B 之间切换，逐轮统计
    * 设置循环次数，实时显示成功率

单次升级调用 tools\\ota_tool\\ota_upload.py；
循环升级调用 tools\\ota_stress\\ota_stress.py（判定标准是版本号是否真的翻转）。

依赖：python + pyserial
"""

import os
import queue
import re
import subprocess
import sys
import threading
import time
import tkinter as tk
from tkinter import filedialog, messagebox, ttk

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))              # 工程根目录
sys.path.insert(0, HERE)

# 不生成 __pycache__（导入 ota_proto 时 Python 默认会写 .pyc 缓存）
sys.dont_write_bytecode = True

import ota_proto                                             # noqa: E402

LOG_DIR = os.path.join(HERE, "log")
# 固件统一从 Keil 编译后的产物目录取
FW_DIR = os.path.join(ROOT, "application", "Project", "firmware", "ota")

try:
    from serial.tools import list_ports
except ImportError:                                          # 没装 pyserial 也能启动，只是没有串口列表
    list_ports = None

VERSION_RE = re.compile(r"(?:outside_version|最终版本):\s*V39\.140\.(\d+)")
RATE_RE = re.compile(r"通过率\s*:\s*([\d.]+)\s*%")
PASS_RE = re.compile(r"通过\s*:\s*(\d+)")
FAIL_RE = re.compile(r"失败\s*:\s*(\d+)")
ROUND_RE = re.compile(r"\[(\d+)/(\d+)\]")


def expected_patch(path):
    """从文件名 V39_140_005.bin 里取 PATCH_VERSION = 5"""
    m = re.search(r"_(\d{3})\.bin$", os.path.basename(path), re.IGNORECASE)
    return int(m.group(1)) if m else None


def pick_newest(d):
    """取目录里最新的一个 .bin"""
    if not os.path.isdir(d):
        return None
    files = [os.path.join(d, n) for n in os.listdir(d) if n.lower().endswith(".bin")]
    if not files:
        return None
    files.sort(key=os.path.getmtime, reverse=True)
    return files[0]


def pick_two(d):
    """从固件目录里挑两个 PATCH 版本不同的 .bin（新的优先）"""
    if not os.path.isdir(d):
        return None, None
    files = [os.path.join(d, n) for n in os.listdir(d) if n.lower().endswith(".bin")]
    files.sort(key=os.path.getmtime, reverse=True)
    first = None
    for f in files:
        p = expected_patch(f)
        if p is None:
            continue
        if first is None:
            first = f
            continue
        if expected_patch(first) != p:
            return first, f
    return (first, None) if first else (None, None)


class OtaGui(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("OTA 上位机")
        self.geometry("820x620")
        self.minsize(720, 520)

        self.worker = None
        self.q = queue.Queue()
        self.stop_flag = False

        self.port = tk.StringVar()
        self.baud = tk.StringVar(value="230400")
        self.file_a = tk.StringVar()
        self.file_b = tk.StringVar()
        self.mode = tk.StringVar(value="single")
        self.target = tk.StringVar(value="A")
        self.rounds = tk.StringVar(value="20")
        self.stop_on_fail = tk.BooleanVar(value=False)
        self.save_log = tk.BooleanVar(value=True)

        self.stat_pass = tk.StringVar(value="-")
        self.stat_fail = tk.StringVar(value="-")
        self.stat_rate = tk.StringVar(value="-")

        self._build()
        self.refresh_ports()
        self.after(100, self._drain)

    # ---------------- 界面 ----------------
    def _build(self):
        pad = dict(padx=6, pady=3)

        frm = ttk.LabelFrame(self, text="串口")
        frm.pack(fill="x", padx=8, pady=6)
        self.cb_port = ttk.Combobox(frm, textvariable=self.port, width=10,
                                   values=self._port_list())
        self.cb_port.pack(side="left", **pad)
        ttk.Button(frm, text="刷新", command=self.refresh_ports).pack(side="left", **pad)
        ttk.Label(frm, text="波特率").pack(side="left", **pad)
        ttk.Combobox(frm, textvariable=self.baud, width=10,
                     values=["230400", "115200", "460800", "921600"]).pack(side="left", **pad)

        frm = ttk.LabelFrame(self, text="固件")
        frm.pack(fill="x", padx=8, pady=6)
        ttk.Label(frm, text="A:").pack(side="left", **pad)
        ttk.Entry(frm, textvariable=self.file_a).pack(side="left", fill="x",
                                                       expand=True, **pad)
        ttk.Button(frm, text="选择", command=lambda: self.pick("A")).pack(side="left", **pad)
        ttk.Label(frm, text="  B:").pack(side="left", **pad)
        ttk.Entry(frm, textvariable=self.file_b).pack(side="left", fill="x",
                                                       expand=True, **pad)
        ttk.Button(frm, text="选择", command=lambda: self.pick("B")).pack(side="left", **pad)

        frm = ttk.LabelFrame(self, text="模式")
        frm.pack(fill="x", padx=8, pady=6)
        ttk.Radiobutton(frm, text="单次升级", variable=self.mode,
                        value="single").pack(side="left", **pad)
        ttk.Radiobutton(frm, text="循环升级", variable=self.mode,
                        value="loop").pack(side="left", **pad)
        ttk.Label(frm, text="目标").pack(side="left", **pad)
        ttk.Combobox(frm, textvariable=self.target, width=4,
                     values=["A", "B"], state="readonly").pack(side="left", **pad)
        ttk.Label(frm, text="循环次数").pack(side="left", **pad)
        ttk.Spinbox(frm, from_=1, to=999, width=6,
                     textvariable=self.rounds).pack(side="left", **pad)
        ttk.Checkbutton(frm, text="首败即停", variable=self.stop_on_fail).pack(side="left", **pad)
        ttk.Checkbutton(frm, text="保存日志文件", variable=self.save_log).pack(side="left", **pad)

        frm = ttk.Frame(self)
        frm.pack(fill="x", padx=8, pady=4)
        self.btn_start = ttk.Button(frm, text="开始", command=self.start)
        self.btn_start.pack(side="left", padx=4)
        self.btn_stop = ttk.Button(frm, text="停止", command=self.stop)
        self.btn_stop.pack(side="left", padx=4)
        self.btn_stop.state(["disabled"])
        self.bar = ttk.Progressbar(frm, length=380, mode="determinate")
        self.bar.pack(side="left", padx=12)
        self.lbl_round = ttk.Label(frm, text="")
        self.lbl_round.pack(side="left", padx=6)

        frm = ttk.LabelFrame(self, text="统计")
        frm.pack(fill="x", padx=8, pady=6)
        ttk.Label(frm, text="  通过: ").pack(side="left")
        ttk.Label(frm, textvariable=self.stat_pass, width=5).pack(side="left")
        ttk.Label(frm, text="  失败: ").pack(side="left")
        ttk.Label(frm, textvariable=self.stat_fail, width=5).pack(side="left")
        ttk.Label(frm, text="  成功率: ").pack(side="left")
        ttk.Label(frm, textvariable=self.stat_rate, width=10).pack(side="left")

        frm = ttk.LabelFrame(self, text="日志")
        frm.pack(fill="both", expand=True, padx=8, pady=6)
        sb = ttk.Scrollbar(frm, orient="vertical")
        self.txt = tk.Text(frm, wrap="none", height=16, state="disabled")
        self.txt.pack(side="left", fill="both", expand=True)
        sb.pack(side="right", fill="y")
        self.txt.configure(yscrollcommand=sb.set)
        sb.configure(command=self.txt.yview)

    # ---------------- 辅助 ----------------
    def _port_list(self):
        if list_ports is None:
            return []
        return sorted(p.device for p in list_ports.comports())

    def refresh_ports(self):
        cur = self.port.get()
        vals = self._port_list()
        self.cb_port.configure(values=vals)
        self.port.set(cur if cur in vals else (vals[0] if vals else ""))
        self.log("串口列表: %s" % (", ".join(vals) if vals else "（未发现）"))

    def pick(self, which):
        path = filedialog.askopenfilename(
            title="选择 OTA 固件 (%s)" % which,
            initialdir=FW_DIR if os.path.isdir(FW_DIR) else ROOT,
            filetypes=[("OTA 固件", "V*_*.bin"), ("所有文件", "*.*")])
        if path:
            if which == "A":
                self.file_a.set(path)
            else:
                self.file_b.set(path)

    def log(self, text):
        self.txt.configure(state="normal")
        self.txt.insert("end", text + "\n")
        self.txt.see("end")
        self.txt.configure(state="disabled")

    # ---------------- 运行 ----------------
    def start(self):
        if self.worker is not None and self.worker.is_alive():
            return

        port = self.port.get().strip()
        if not port:
            messagebox.showwarning("提示", "请选择串口")
            return
        try:
            baud = int(self.baud.get() or 230400)
        except ValueError:
            messagebox.showwarning("提示", "波特率填错了")
            return
        # 每次运行都写一份日志到 log\，取消勾选才不落盘
        log_dir = LOG_DIR if self.save_log.get() else None
        if log_dir:
            os.makedirs(log_dir, exist_ok=True)

        if self.mode.get() == "single":
            fw = self.file_a.get().strip() if self.target.get() == "A" \
                else self.file_b.get().strip()
            if not fw or not os.path.isfile(fw):
                messagebox.showwarning("提示", "请选择存在的 OTA 固件")
                return
            self._reset_ui()
            self.log("-" * 60)
            self.log("单次升级: %s" % os.path.basename(fw))
            self.log("-" * 60)
            self.worker = threading.Thread(target=self._single_worker,
                                          args=(port, fw, baud, log_dir),
                                          daemon=True)
            self.worker.start()
            return

        # 循环模式
        fa = self.file_a.get().strip()
        fb = self.file_b.get().strip()
        if not fa or not fb:
            # 没指定就从固件目录里自动挑两个版本不同的
            auto_a, auto_b = pick_two(FW_DIR)
            fa = fa or auto_a
            fb = fb or auto_b
        if not (fa and fb and os.path.isfile(fa) and os.path.isfile(fb)):
            messagebox.showwarning("提示", "请选择 A、B 两个 OTA 固件")
            return
        va, vb = expected_patch(fa), expected_patch(fb)
        if va is None or vb is None or va == vb:
            messagebox.showwarning("提示", "A、B 的 PATCH 版本必须不同")
            return
        try:
            rounds = int(self.rounds.get())
        except ValueError:
            messagebox.showwarning("提示", "循环次数必须是整数")
            return

        self._reset_ui()
        self.log("-" * 60)
        self.log("循环升级 %d 轮   A=%s  B=%s" % (rounds, os.path.basename(fa),
                                              os.path.basename(fb)))
        self.log("-" * 60)
        self.worker = threading.Thread(target=self._loop_worker,
                                      args=(port, rounds,
                                            fa, fb, va, vb, log_dir), daemon=True)
        self.worker.start()

    def _reset_ui(self):
        self.stop_flag = False
        self.bar["value"] = 0
        self.stat_pass.set("-")
        self.stat_fail.set("-")
        self.stat_rate.set("-")
        self.lbl_round.configure(text="")
        self.btn_start.state(["disabled"])
        self.btn_stop.state(["!disabled"])

    def _single_worker(self, port, fw, baud, log_dir):
        try:
            # 只显示进度和结果：关闭 TX/RX 明细和门锁调试日志
            ota_proto.run_upload(port, fw, baud=baud,
                                 log_dir=log_dir, dev_log=False,
                                 trace=False, quiet=False, sink=self._put_line)
            self.q.put(("info", "升级成功"))
        except ota_proto.OtaError as exc:
            self.q.put(("info", "升级失败: %s" % exc))
        except Exception as exc:
            self.q.put(("info", "异常: %s" % exc))
        self.q.put(("end", None))

    def _loop_worker(self, port, rounds, fa, fb, va, vb, log_dir):
        cur = None
        unknown = True
        passed = failed = 0
        t_loop = time.time()
        for i in range(1, rounds + 1):
            if self.stop_flag:
                break
            if cur == va:
                fw, expect = fb, vb
            elif cur == vb:
                fw, expect = fa, va
            else:
                fw, expect = (fb, vb) if (i % 2 == 1) else (fa, va)
            self.q.put(("round", (i, rounds, expect)))
            try:
                # 只显示进度和结果：关闭 TX/RX 明细和门锁调试日志
                ota_proto.run_upload(port, fw, log_dir=log_dir,
                                     dev_log=False, trace=False, quiet=False,
                                     sink=self._put_line)
                err = ""
            except ota_proto.OtaError as exc:
                err = str(exc)
            except Exception as exc:
                err = "异常: %s" % exc
            got = ota_proto.LAST_VERSION
            gs = ("V39.140.%03d" % got) if got is not None else "未知"
            if got is not None and got == expect:
                ok, reason = True, ("" if not err else "版本已翻转(%s)" % err)
            elif err:
                ok, reason = False, err
            elif got is None:
                ok, reason = False, "未收到 outside_version"
            else:
                ok, reason = False, "版本未翻转，仍是 %s" % gs
            priming = unknown and ok
            unknown = False
            if got is not None:
                cur = got
            if priming:
                self.q.put(("info", "[%d/%d] 目标 V39.140.%03d 打底 当前 %s（不计入）"
                            % (i, rounds, expect, gs)))
            elif ok:
                passed += 1
                self.q.put(("info", "[%d/%d] 目标 V39.140.%03d 通过 当前 %s"
                            % (i, rounds, expect, gs)))
            else:
                failed += 1
                self.q.put(("info", "[%d/%d] 目标 V39.140.%03d 失败 当前 %s <- %s"
                            % (i, rounds, expect, gs, reason)))
                if self.stop_on_fail.get():
                    self.q.put(("info", "按「首败即停」中止"))
                    break
            t = passed + failed
            self.q.put(("stat", (passed, failed, passed * 100.0 / t if t else 0.0)))

            # 轮间隔 5 秒：给设备重启和链路恢复留时间
            if i < rounds and not self.stop_flag:
                self.q.put(("info", "等待 5 秒后开始下一轮..."))
                time.sleep(5)
        self.q.put(("info", "循环结束：共 %d 轮，通过 %d，失败 %d，总耗时 %.1f 秒"
                    % (passed + failed, passed, failed, time.time() - t_loop)))
        self.q.put(("end", None))

    def _put_line(self, line):
        self.q.put(("line", line))

    def stop(self):
        self.stop_flag = True
        self.log(">>> 将在当前一轮结束后停止")

    def _drain(self):
        try:
            while True:
                kind, data = self.q.get_nowait()
                if kind in ("line", "info"):
                    self.log(data)
                elif kind == "round":
                    i, n, e = data
                    self.bar["maximum"] = n
                    self.bar["value"] = i
                    self.lbl_round.configure(text="%d / %d" % (i, n))
                elif kind == "stat":
                    p, f, rate = data
                    self.stat_pass.set(str(p))
                    self.stat_fail.set(str(f))
                    self.stat_rate.set("%.1f %%" % rate)
                elif kind == "end":
                    self.btn_start.state(["!disabled"])
                    self.btn_stop.state(["disabled"])
                    self.worker = None
        except queue.Empty:
            pass
        self.after(100, self._drain)


if __name__ == "__main__":
    OtaGui().mainloop()
