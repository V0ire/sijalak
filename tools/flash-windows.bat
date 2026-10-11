@echo off
REM =====================================================================
REM  Si Jalak Bedas - Windows FLASH (double-click launcher)
REM  Requires installed ESP-IDF. Builds first; never picks a COM port automatically.
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
echo   Langkah: cek ESP-IDF terpasang -^> build -^> validasi output -^> flash
echo ============================================================
echo.
echo [1] Build + flash (port COM harus diberikan)
echo [2] Build + flash + buka serial monitor
echo [3] Build saja (jangan flash)
echo [4] Keluar
echo.
choice /C 1234 /N /M "Pilih [1-4]: "
if errorlevel 5 exit /b 1
if errorlevel 4 exit /b 0
if errorlevel 3 goto buildonly
if errorlevel 2 goto monitor
if errorlevel 1 goto flash
exit /b 1

:flash
set "JALAK_FLASH_SCRIPT=%PS1%"
powershell -NoProfile -ExecutionPolicy Bypass -Command "$p = Read-Host 'Port board terverifikasi (mis. COM5)'; & $env:JALAK_FLASH_SCRIPT -Port $p; exit $LASTEXITCODE"
set "RC=%ERRORLEVEL%"
goto result
:monitor
set "JALAK_FLASH_SCRIPT=%PS1%"
powershell -NoProfile -ExecutionPolicy Bypass -Command "$p = Read-Host 'Port board terverifikasi (mis. COM5)'; & $env:JALAK_FLASH_SCRIPT -Port $p -Monitor; exit $LASTEXITCODE"
set "RC=%ERRORLEVEL%"
goto result
:buildonly
powershell -NoProfile -ExecutionPolicy Bypass -File "%PS1%" -NoFlash
set "RC=%ERRORLEVEL%"
:result

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
