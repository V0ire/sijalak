@echo off
REM =====================================================================
REM  Si Jalak Bedas - Windows installer/launcher (double-click this file)
REM  Runs the PowerShell installer with execution policy bypassed for this
REM  one process only. Nothing global is changed.
REM =====================================================================
setlocal
set "PS1=%~dp0install-windows.ps1"

if not exist "%PS1%" (
    echo [ERROR] install-windows.ps1 not found next to this .bat
    pause
    exit /b 1
)

echo ============================================================
echo   Si Jalak Bedas - Windows setup
echo   This will install ESP-IDF v5.4 (esp32s3) if not present,
echo   build host tests, and build the firmware target.
echo ============================================================
echo.
pause

powershell -NoProfile -ExecutionPolicy Bypass -File "%PS1%" %*

echo.
echo ============================================================
echo   Finished. Check the messages above for any [MISS]/[WARN].
echo ============================================================
pause
endlocal
