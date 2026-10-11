# Si Jalak Bedas — Panduan Build di Windows

Host Linux telah diuji; itu bukan bukti build Xtensa, Windows, atau hardware.
Default firmware hanya OLED, touch, speaker TX 16 kHz dan SD. Tidak ada mic,
Wi-Fi, HTTP atau voice/cloud dalam dependency graph lokal.

---

## Bagian 1 — Backend (bisa langsung, tanpa ESP-IDF)

Backend Starlette berjalan di Windows native. Di **PowerShell**:

```powershell
cd sijalak-bedas\backend
py -m pip install starlette uvicorn pytest httpx anyio
py -m pytest -q                       # periksa exit code, jangan mengandalkan jumlah lama
py -m jalak_backend --port 8787       # server offline (stub provider)
```

Uji cepat (terminal kedua):
```powershell
# buat sesi
curl.exe -s -X POST http://127.0.0.1:8787/v1/session -H "x-device-token: dev-token" -H "x-device-id: dev"
# (salin session_id lalu submit turn — lihat README backend)
```

---

## Bagian 2 — Firmware ESP32-S3

### Opsi A — WSL2 (rekomendasi; paling dekat dengan CI Linux)

1. **PowerShell (sekali saja, sebagai Admin):**
   ```powershell
   wsl --install -d Ubuntu
   ```
   Reboot bila diminta, lalu buka "Ubuntu" dari Start Menu.

2. **Di dalam Ubuntu WSL:**
   ```bash
   sudo apt update
   sudo apt install -y git cmake ninja-build python3 python3-pip python3-venv \
                       gcc g++ flex bison gperf libffi-dev libssl-dev dfu-util libusb-1.0-0
   ```

3. **Clone repo + ESP-IDF v5.4:**
   ```bash
    git clone https://github.com/V0ire/sijalak.git
   git clone -b v5.4 --recursive https://github.com/espressif/esp-idf.git ~/esp/esp-idf
   cd ~/esp/esp-idf && ./install.sh esp32s3 && . ./export.sh
   ```

4. **Build host test (cepat, tanpa board):**
   ```bash
    cd ~/sijalak
    mkdir -p .buildtmp
    export TMPDIR="$PWD/.buildtmp"
    cmake -S firmware/test -B "$TMPDIR/fw_build" -G Ninja -DCMAKE_BUILD_TYPE=Debug
   cmake --build "$TMPDIR/fw_build"
    ctest --test-dir "$TMPDIR/fw_build" --output-on-failure
   ```

5. **Build target ESP32-S3 (butuh toolchain dari langkah 3):**
   ```bash
    cd ~/sijalak
    python "$IDF_PATH/tools/idf.py" -C firmware -B .buildtmp/idf-s3-wsl -D IDF_TARGET=esp32s3 build
   ```
    Output: `.buildtmp/idf-s3-wsl/sijalak_bedas.bin`. Jangan pakai build dir host
    untuk target atau menjalankan `set-target` berulang: aksi itu menjalankan fullclean.

    > Final target link masih belum diverifikasi dengan compiler Xtensa asli.
    > Tes baru compile real `ESP_PLATFORM` HAL body dengan mock boundary, bukan
    > TU kosong, tetapi tetap bukan SDK/hardware. HTTP transport tidak di-build.

6. **Flash ke board (butuh board + USB):**
   ```bash
    python "$IDF_PATH/tools/idf.py" -C firmware -B .buildtmp/idf-s3-wsl -p /dev/ttyUSB0 flash monitor
   ```
   Di WSL, USB serial perlu `usbipd-win` untuk di-attach ke WSL. **Lebih mudah
   flash dari Windows native (Opsi B).**

### Opsi B — ESP-IDF Windows Installer (native, terbaik untuk flashing)

1. Download **ESP-IDF Tools Installer v5.4** dari https://dl.espressif.com/dl/esp-idf/
2. Jalankan installer; pilih target **esp32s3**. Ini memasang Python + toolchain
   Xtensa otomatis (tanpa WSL, tanpa sudo).
3. Buka shortcut **"ESP-IDF 5.4 PowerShell"** dari Start Menu.
4. Di dalamnya:
   ```powershell
    cd C:\path\ke\sijalak
    . "$HOME\esp\esp-idf\export.ps1"
    powershell -NoProfile -ExecutionPolicy Bypass -File .\tools\flash-windows.ps1 -NoFlash -SkipAssets
    # Setelah port board diverifikasi di Device Manager:
    powershell -NoProfile -ExecutionPolicy Bypass -File .\tools\flash-windows.ps1 -Port COM3 -Monitor -SkipAssets
   ```

Badge checklist:
- [ ] Backend `pytest` hijau
- [ ] Host `ctest` hijau + sanitizer hijau
- [ ] `idf.py build` sukses → `.bin` terbentuk
- [ ] (butuh board) flash + monitor berjalan

Hal yang **belum** divalidasi: pin I2C/I2S/touch/SD, alamat/controller OLED,
clocking dan target toolchain. RX `-1` memang disengaja untuk **menonaktifkan mic**,
bukan placeholder yang perlu diisi. Panduan konfigurasi build dir terpisah dan
menuconfig persisten: [FLASH-WINDOWS.md](FLASH-WINDOWS.md).

Host Windows: `powershell -ExecutionPolicy Bypass -File .\tools\build-host-tests.ps1`.
Butuh compiler host C++20; firmware IDF sendiri tidak memerlukan MSVC/MinGW.
Parser/mock Windows: `powershell -ExecutionPolicy Bypass -File .\tools\test-windows.ps1`.
Tes tersebut hanya berjalan bila PowerShell sudah terpasang; source guards saja
tidak membuktikan skrip dapat diparse/dijalankan.
