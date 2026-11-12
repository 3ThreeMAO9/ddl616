@echo off
rem ============================================================
rem  OTA 反复压测
rem  固件从本目录的 fw_a / fw_b 各取一个（PATCH 版本必须不同）
rem  日志落在本目录的 log\
rem ============================================================
setlocal

cd /d "%~dp0"

set PORT=COM7
set ROUNDS=20

if not "%~1"=="" set PORT=%~1
if not "%~2"=="" set ROUNDS=%~2

echo.
echo   串口  : %PORT%
echo   轮数  : %ROUNDS%
echo   固件  : fw_a / fw_b
echo.

python ota_stress.py --port %PORT% --rounds %ROUNDS% --quiet

echo.
pause
endlocal
