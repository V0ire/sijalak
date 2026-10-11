@echo off
REM =====================================================================
REM  Si Jalak Bedas - Windows installer/launcher (double-click this file)
REM  Runs the PowerShell installer with execution policy bypassed for this
REM  one process only. Nothing global is changed.
REM
REM  Run this from the REPO ROOT (folder containing firmware\ backend\ tools\).
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
echo   This expects to be run from the repo root:
echo     %~dp0..
echo ============================================================
echo.
pause

powershell -NoProfile -ExecutionPolicy Bypass -File "%PS1%" %*
set "RC=%ERRORLEVEL%"

echo.
if "%RC%"=="0" (
    echo ============================================================
    echo   Finished OK. Read the messages above for any [WARN].
    echo ============================================================
) else (
    echo ============================================================
    echo   STOPPED with error %RC%. Nothing to flash.
    echo   Scroll up to the FIRST [FAIL] line and fix that.
    echo ============================================================
)
pause
exit /b %RC%
