# Si Jalak Bedas — Panduan Build di Windows

Ada **dua cara**. WSL2 paling mirip environment Linux tempat proyek ini dikembangkan
(dan sudah terbukti `ctest 5/5`, `pytest 46`). ESP-IDF Windows Installer paling mulus
untuk flashing ke board.

---

## Bagian 1 — Backend (bisa langsung, tanpa ESP-IDF)

Backend Starlette berjalan di Windows native. Di **PowerShell**:

```powershell
cd sijalak-bedas\backend
py -m pip install starlette uvicorn pytest httpx anyio
py -m pytest -q                       # harapkan: 46 passed
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
   git clone https://github.com/USERNAME/sijalak-bedas.git
   git clone -b v5.4 --recursive https://github.com/espressif/esp-idf.git ~/esp/esp-idf
   cd ~/esp/esp-idf && ./install.sh esp32s3 && . ./export.sh
   ```

4. **Build host test (cepat, tanpa board):**
   ```bash
   cd ~/sijalak-bedas/firmware
   export TMPDIR="$PWD/.buildtmp"
   cmake -S test -B "$TMPDIR/fw_build" -G Ninja -DCMAKE_BUILD_TYPE=Debug
   cmake --build "$TMPDIR/fw_build"
   ctest --test-dir "$TMPDIR/fw_build"        # harapkan: 100% tests passed (5)
   bash test/run_sanitizers.sh               # ASan/UBSan, harapkan semua PASSED
   ```

5. **Build target ESP32-S3 (butuh toolchain dari langkah 3):**
   ```bash
   cd ~/sijalak-bedas/firmware
   idf.py set-target esp32s3
   idf.py build
   ```
   Output: `build/sijalak_bedas.bin`

   > Catatan: 4 driver IDF (`i2c_oled_display`, `touch_input`, `i2s_audio_device`,
   > `esp_http_transport`) dan `app_main.cpp` **belum pernah dikompilasi toolchain
   > nyata**. Jika `idf.py build` error, kirim log-nya — perbaikan API IDF biasanya
   > kecil (perubahan signature header driver).

6. **Flash ke board (butuh board + USB):**
   ```bash
   idf.py -p /dev/ttyUSB0 flash monitor
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
   git clone https://github.com/USERNAME/sijalak-bedas.git
   cd sijalak-bedas\firmware
   idf.py set-target esp32s3
   idf.py build
   idf.py -p COM3 flash monitor     # ganti COM3 dengan port board Anda
   ```

Badge checklist:
- [ ] Backend `pytest` hijau
- [ ] Host `ctest` hijau + sanitizer hijau
- [ ] `idf.py build` sukses → `.bin` terbentuk
- [ ] (butuh board) flash + monitor berjalan

Hal yang **belum** divalidasi sampai Anda punya board: pin I2C/I2S/touch, alamat I2C
OLED, endpoint HTTPS nyata. `app_main.cpp` memakai pin placeholder (-1) yang **wajib**
diganti sesuai wiring board Anda.
