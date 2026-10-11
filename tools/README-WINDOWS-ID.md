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
    . "$HOME\esp\esp-idf\export.ps1"
    powershell -NoProfile -ExecutionPolicy Bypass -File .\tools\flash-windows.ps1 -NoFlash -SkipAssets
   ```
5. **Flash ke board** (board tercolok USB):
   ```powershell
    powershell -NoProfile -ExecutionPolicy Bypass -File .\tools\flash-windows.ps1 -Port COM3 -Monitor -SkipAssets
   ```

---

## Cara otomatis (tanpa installer Espressif)

Kalau Anda sudah punya **Git + Python**, **klik dua kali**:

```
tools\install-windows.bat
```

Script itu akan:
- Menjalankan test backend Python,
- Men-clone + memasang ESP-IDF v5.4 (esp32s3) kalau belum ada,
- Meng-build target ESP32-S3.
- Berhenti pada error native pertama, termasuk aktivasi, versi/toolchain dan output.
- **Mengunduh** dependensi/SDK. Jangan gunakan installer untuk build-only offline;
  gunakan `flash-windows.ps1 -NoFlash` dengan SDK yang sudah terpasang.

Test host terpisah membutuhkan CMake, Ninja dan compiler C++20 (MSVC/MinGW).
Tidak ada opsi `-AutoInstall`/winget otomatis; pasang prasyarat yang Anda perlukan
secara eksplisit. Untuk host test:

```powershell
powershell -ExecutionPolicy Bypass -File .\tools\build-host-tests.ps1
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
- **Driver hardware** (OLED I2C, amp I2S, touch, SD) dan `app_main` **BELUM
  divalidasi di board nyata**. Mic RX `-1` sengaja dinonaktifkan; jangan diisi.
- Keberhasilan `idf.py build` **bukan** bukti perangkat berfungsi.
- `.bat` menjalankan child PowerShell. Untuk build manual di terminal induk:
  `. "$HOME\esp\esp-idf\export.ps1"; python "$env:IDF_PATH\tools\idf.py" build`
  dari folder `firmware`. Jangan mengandalkan asosiasi file `.py`/bare `idf.py`.
- Skrip memakai `.buildtmp\idf-esp32s3` dan `.buildtmp\sdkconfig-esp32s3`.
  Tidak membersihkan cache, mengganti target berulang, atau memilih COM pertama.
  Untuk cache salah, pilih `-BuildDir` baru; konfigurasi OLED/touch tetap dipertahankan.
- Output hilang/kosong (`.bin`, flash args, image yang direferensikan) membuat skrip
  gagal dan tidak flash. Lihat `.buildtmp\flash-windows.log` atau `install-windows.log`.
- Build target/Windows compiler health masih belum terverifikasi; source lokal
  punya dependency untracked, jadi checkout remote belum lengkap. Tidak ada push.
- Panduan lengkap dan menuconfig: [FLASH-WINDOWS.md](FLASH-WINDOWS.md).
