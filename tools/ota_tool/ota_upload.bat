@echo off
chcp 65001 >nul
setlocal enabledelayedexpansion

rem ============================================================
rem  OTA 串口上传（加密）
rem    双击        : 自动选工程根目录最新的 V*_*.bin
rem    命令行      : ota_upload.bat COM3
rem                  ota_upload.bat COM3 D:\xxx.bin
rem    拖拽        : 把 .bin 拖到这个文件上
rem  需要 python + pyserial
rem ============================================================

set "PORT=COM7"
set "FILE="

if /i "%~x1"==".bin" (
    set "FILE=%~1"
) else (
    if not "%~1"=="" set "PORT=%~1"
)
if not "%~2"=="" set "FILE=%~2"

rem 没指定就依次找：ota_stress\fw_a -> ota_stress\fw_b -> firmware\ota
if "%FILE%"=="" (
    for /f "delims=" %%f in ('dir /b /o-d "%~dp0..\ota_stress\fw_a\V*_*.bin" 2^>nul') do (
        set "FILE=%~dp0..\ota_stress\fw_a\%%f"
        goto :gotfile
    )
    for /f "delims=" %%f in ('dir /b /o-d "%~dp0..\ota_stress\fw_b\V*_*.bin" 2^>nul') do (
        set "FILE=%~dp0..\ota_stress\fw_b\%%f"
        goto :gotfile
    )
    for /f "delims=" %%f in ('dir /b /o-d "%~dp0..\..\firmware\ota\V*_*.bin" 2^>nul') do (
        set "FILE=%~dp0..\..\firmware\ota\%%f"
        goto :gotfile
    )
)
:gotfile

if not exist "%FILE%" (
    echo.
    echo [ERROR] 没找到 OTA 固件
    echo         查找顺序: ota_stress\fw_a  -^>  ota_stress\fw_b  -^>  firmware\ota
    echo.
    pause
    exit /b 1
)

echo.
echo ============================================================
echo  PORT: %PORT%
echo  FILE: %FILE%
echo ============================================================
echo.

python "%~dp0ota_upload.py" --port %PORT% --file "%FILE%"

echo.
echo ============================================================
pause
