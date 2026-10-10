# Si Jalak Bedas — Setup untuk Windows

Panduan singkat. Anggap komputer **belum** ada ESP-IDF.

---

## Cara tercepat

1. **Install Git** (kalau belum): https://git-scm.com/download/win
2. **Install "ESP-IDF Tools Installer"** — cara paling gampang:
   - Buka https://dl.espressif.com/dl/esp-idf/
   - Pilih **v5.4** → **ESP-IDF Windows Installer**
   - Jalankan; centang target **esp32s3**. Installer ini otomatis memasang
     Python, CMake, Ninja, dan toolchain Xtensa (butuh ~3 GB, 10–20 menit).
   - Setelah selesai, akan ada shortcut **"ESP-IDF 5.4 PowerShell"** di Start Menu.
3. **Clone repo** (di "ESP-IDF 5.4 PowerShell"):
   ```powershell
   cd $HOME
   git clone https://github.com/V0ire/sijalak.git
   cd sijalak
   ```
4. **Build firmware**:
   ```powershell
   cd firmware
   idf.py set-target esp32s3
   idf.py build
   ```
5. **Flash ke board** (board tercolok USB):
   ```powershell
   idf.py -p COM3 flash monitor      # ganti COM3 -> lihat di Device Manager
   ```

---

## Cara otomatis (tanpa installer Espressif)

Kalau Anda sudah punya **Git + CMake + Ninja + Visual Studio Build Tools**
(atau MinGW-w64), cukup **klik dua kali**:

```
tools\install-windows.bat
```

Script itu akan:
- Menjalankan test backend Python,
- Men-clone + memasang ESP-IDF v5.4 (esp32s3) kalau belum ada,
- Menjalankan test host (CTest),
- Meng-build target ESP32-S3.

Butuh prasyarat yang belum ada? Jalankan:

```powershell
powershell -ExecutionPolicy Bypass -File .\tools\install-windows.ps1 -AutoInstall
```

---

## Menjalankan backend saja (tanpa board)

Di PowerShell biasa:

```powershell
cd sijalak\backend
py -m pip install starlette uvicorn pytest httpx anyio
py -m pytest -q
py -m jalak_backend --port 8787
```

Buka http://127.0.0.1:8787 — server offline (provider tiruan, tanpa internet).

---

## Catatan penting (jujur)

- **Backend** bisa langsung dicoba: sudah lolos test end-to-end.
- **Test firmware host** (CTest) jalan tanpa board.
- **Driver hardware** (OLED I2C, mic/amp I2S, touch) dan `app_main` **BELUM
  divalidasi di board nyata**. Pin di `app_main.cpp` masih placeholder `-1` —
  ganti sesuai wiring Anda.
- Keberhasilan `idf.py build` **bukan** bukti perangkat berfungsi.
- Kalau `idf.py build` error: kirim log-nya ke pengembang. Biasanya hanya perlu
  penyesuaian kecil pada header driver IDF.
