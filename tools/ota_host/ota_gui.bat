@echo off
chcp 65001 >nul
setlocal
rem ============================================================
rem  OTA Host Tool (tkinter GUI)
rem    Pick COM port / firmware A and B / single or loop upgrade
rem    Success rate is shown in the GUI.
rem  NOTE: keep this file ASCII-only. cmd.exe can lose its file
rem        offset when a UTF-8 batch switches codepage mid-file,
rem        then it executes the tail of a comment line as command.
rem  Requires: python + pyserial
rem ============================================================

cd /d "%~dp0"

rem Launch with pythonw so only the GUI is shown (no cmd console)
start "" pythonw "%~dp0ota_gui.py"
if errorlevel 1 (
    rem Fall back to python if pythonw is unavailable (console will show)
    python "%~dp0ota_gui.py"
)

endlocal
