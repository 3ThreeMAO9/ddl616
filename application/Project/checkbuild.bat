@echo off
rem ============================================================
rem checkbuild.bat - portable Keil build wrapper
rem Auto-finds the Keil-generated batch file in this directory
rem by its UPPERCASE extension (*.BAT, e.g. 60308.BAT), runs it
rem with real-time console output, then summarizes:
rem   Warnings in YELLOW first, Errors in RED after.
rem
rem NOTE: this file's own extension must stay lowercase ".bat",
rem       and build BATs must use uppercase ".BAT" (Keil default).
rem Usage: copy next to any xx.BAT, then run: checkbuild.bat
rem ============================================================
cd /d %~dp0

rem ---- find the build BAT (uppercase .BAT extension only) ----
set "BUILD_BAT="
for /f "delims=" %%f in ('powershell -NoProfile -Command "Get-ChildItem -File -Filter *.bat | Where-Object { $_.Name -cmatch '\.BAT$' -and $_.Name -cne 'checkbuild.bat' } | Select-Object -ExpandProperty Name"') do (
    if not defined BUILD_BAT set "BUILD_BAT=%%f"
)

if not defined BUILD_BAT (
    echo.
    echo ================ ERROR: no *.BAT build script found ================
    echo Looked in: %cd%
    exit /b 1
)

echo Build script: %BUILD_BAT%
echo.

powershell -NoProfile -Command "cmd /c .\%BUILD_BAT% 2>&1 | Tee-Object -Variable lines | Out-Default; $lines | Set-Content .\build.log -Encoding ASCII"

rem ---- Check 1: compile/link errors (ArmCC "Error: #xxx", ArmLink "Fatal error:") ----
findstr /i /c:"error:" build.log >nul
if %errorlevel%==0 goto failed

rem ---- Check 2: link success marker (ArmLink prints "Program Size" on success) ----
findstr /c:"Program Size" build.log >nul
if errorlevel 1 goto nolink

echo.
findstr /c:"Program Size" build.log
call :summary
powershell -NoProfile -Command "Write-Host '================ BUILD OK~~~~ ================' -ForegroundColor Green"
exit /b 0

:failed
echo.
call :summary
echo ==============================================
echo See build.log for details
powershell -NoProfile -Command "Write-Host '================ BUILD FAILED!!!! ================' -ForegroundColor Red"
exit /b 1

:nolink
echo.
call :summary
echo "Program Size" marker not found. See build.log for details
powershell -NoProfile -Command "Write-Host '================ BUILD FAILED!!! (no link) ================' -ForegroundColor Red"
exit /b 1

:summary
rem ---- colored summary: Warnings (yellow) first, then Errors (red);
rem      Errors section auto-hidden when there are none (BUILD OK) ----
powershell -NoProfile -Command "$w = @(Select-String -Path .\build.log -Pattern 'warning:'); $e = @(Select-String -Path .\build.log -Pattern 'error:'); Write-Host ''; Write-Host ('---- Warnings ---- [' + $w.Count + ']') -ForegroundColor Yellow; if ($w.Count) { $w | ForEach-Object { Write-Host $_.Line -ForegroundColor Yellow } } else { Write-Host '(none)' }; if ($e.Count) { Write-Host ''; Write-Host ('---- Errors ---- [' + $e.Count + ']') -ForegroundColor Red; $e | ForEach-Object { Write-Host $_.Line -ForegroundColor Red } }"
goto :eof
