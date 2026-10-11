@echo off
REM =====================================================================
REM  Si Jalak Bedas - Windows FLASH (double-click launcher)
REM  Runs tools\flash-windows.ps1: install ESP-IDF (if missing) -> build -> flash.
REM
REM  Tips:
REM    - Pastikan board tercolok dengan kabel DATA dan sudah terdeteksi COM.
REM    - Kalau gagal konek: tahan BOOT, tekan RST, lepas BOOT, lalu jalankan lagi.
REM =====================================================================
setlocal
set "PS1=%~dp0flash-windows.ps1"

if not exist "%PS1%" (
    echo [ERROR] flash-windows.ps1 tidak ada di folder ini
    pause
    exit /b 1
)

echo ============================================================
echo   Si Jalak Bedas - FLASH ke ESP32-S3 (Windows)
echo   Langkah: cek prasyarat -^> install ESP-IDF -^> build -^> flash
echo   Ini bisa memakan 15-30 menit pada percobaan pertama.
echo ============================================================
echo.
echo [1] Flash saja (build sudah pernah sukses)
echo [2] Flash + buka serial monitor
echo [3] Build saja (jangan flash)
echo [4] Keluar
echo.
set /p CHOICE="Pilih [1-4]: "

if "%CHOICE%"=="1" set "ARGS="
if "%CHOICE%"=="2" set "ARGS=-Monitor"
if "%CHOICE%"=="3" set "ARGS=-NoFlash"
if "%CHOICE%"=="4" exit /b 0

powershell -NoProfile -ExecutionPolicy Bypass -File "%PS1%" %ARGS%
set "RC=%ERRORLEVEL%"

echo.
if "%RC%"=="0" (
    echo ============================================================
    echo   Selesai. Baca pesan di atas untuk peringatan [WARN].
    echo ============================================================
) else (
    echo ============================================================
    echo   STOP pada error %RC%. Lihat baris [FAIL] PERTAMA di atas.
    echo ============================================================
)
pause
exit /b %RC%
