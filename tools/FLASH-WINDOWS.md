# Panduan Flash ke ESP32-S3 di Windows

Tujuan: build demo lokal ESP32-S3 lalu flash ke port yang Anda verifikasi.
Perintah **PowerShell** kecuali disebut lain. `flash-windows.ps1` tidak memasang
SDK, tidak clone repo, dan tidak memilih COM secara otomatis.

> **Penting (jujur):** Firmware ini **belum pernah di-build dengan toolchain
> ESP-IDF asli** dan **belum pernah diuji di board**. Kode sudah lulus host test
> dan behavioral mock HAL, bukan bukti Windows/SDK/hardware. Compiler probe
> sekarang memakai `STATIC_LIBRARY`: compile/archive tetap diuji, final link
> firmware tetap wajib berhasil. Solusi ini belum diuji pada toolchain Windows
> yang menghasilkan error big-endian `crt0.o`; jangan menyimpulkan compiler rusak.

---

## 0. Yang Anda butuhkan

- Board **ESP32-S3** (yang dipakai: varian N16R8 — 16 MB flash, 8 MB PSRAM).
- **OLED SSD1306/SH1106 128×64** I2C.
- Tombol / modul touch dengan output digital di GPIO.
- **MAX98357A** (speaker) — opsional, untuk suara.
- **microSD + card reader** — opsional, untuk animasi & pop sound.
- Kabel **USB data** (bukan charge-only).
- Windows 10/11, ruang disk **±4 GB**.

Wiring yang dipakai firmware (bisa diubah di kode):

| Fungsi | Pin |
|---|---|
| OLED I2C | SDA=8, SCL=9 |
| Touch/button | GPIO 2 |
| Amp MAX98357A | BCLK=4, LRC=5, DIN=6 |
| microSD (SPI) | CS=10, MOSI=11, SCK=12, MISO=13 |
| Mic INMP441 | **tidak dipakai** di demo ini |

---

## 1. Install Git

1. Unduh https://git-scm.com/download/win → jalankan installer (default OK).
2. Verifikasi di PowerShell:
   ```powershell
   git --version
   ```

## 2. Install ESP-IDF v5.4 (cara termudah)

1. Unduh **ESP-IDF Tools Installer v5.4**: https://dl.espressif.com/dl/esp-idf/
   pilih baris **v5.4** → **ESP-IDF Windows Installer**.
2. Jalankan. Saat diminta komponen: **centang minimal `esp32s3`**.
   (Installer memasang Python, CMake, Ninja, dan toolchain Xtensa otomatis.)
3. Setelah selesai, Start Menu akan punya shortcut **"ESP-IDF 5.4 PowerShell"**.

> **Catatan Windows + USB:** kalau board tidak terdeteksi, install driver
> **CP210x** atau **CH9102** dari Espressif (instaler biasanya sudah menyertakan,
> kadang perlu manual dari https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers).

## 3. Ambil kode

Di **"ESP-IDF 5.4 PowerShell"**:
```powershell
cd $HOME
git clone https://github.com/V0ire/sijalak.git
cd sijalak
```
Kalau sudah punya repo-nya (folder `sijalak2`), lewati langkah ini dan masuk ke
folder itu.

## 4. (Opsional) Siapkan microSD untuk animasi & suara

Lewati kalau Anda mau menjalankan tanpa SD (demo tetap jalan dengan mata bawaan).

1. Format microSD sebagai **FAT32**.
2. Di folder repo, jalankan konverter (butuh Python + Pillow):
   ```powershell
   py -m pip install pillow
   py tools\mochi_assets.py --mochi C:\path\ke\watcher-mochi-ref --out firmware\preview\assets
   ```
3. Copy isi `firmware\preview\assets\` ke kartu SD, sehingga di kartu ada:
   ```
   \frames\happy.bin
   \frames\love.bin
   \frames\wink.bin
   \frames\sleepy.bin
   \frames\index.json
   \pop.pcm
   ```
4. Masukkan kartu ke board.

> **Aset Mochi (GIF Dasai + pop sound) berlisensi pihak ketiga untuk pemakaian
> pribadi.** Jangan commit atau sebarkan. Konverter hanya menulis ke folder lokal
> yang sudah di-ignore git.

## 5. Aktivasi & build tanpa flash

```powershell
. "$HOME\esp\esp-idf\export.ps1"
python "$env:IDF_PATH\tools\idf.py" --version
powershell -NoProfile -ExecutionPolicy Bypass -File .\tools\flash-windows.ps1 -NoFlash -SkipAssets
```

Ganti `-IdfPath` bila SDK terpasang di lokasi lain. Skrip memeriksa versi 5.4.x,
Python aktif, source SDK, dan compiler S3. Adanya `export.ps1` saja bukan bukti
instalasi selesai. Aktivasi di proses `.bat`/child PowerShell **tidak diwariskan
kembali** ke terminal induk; dot-source ulang untuk perintah manual.

Build skrip berada di `<repo>\.buildtmp\idf-esp32s3`, bukan `firmware\build`.
Konfigurasi persisten: `<repo>\.buildtmp\sdkconfig-esp32s3`. Tidak ada `set-target`,
`fullclean`, atau penghapusan cache. Folder lama tanpa marker kepemilikan atau
cache dari source/target/SDK/compiler lain ditolak; pilih **folder baru** dengan
`-BuildDir`, bukan hapus folder lama. Gunakan `-Sdkconfig` baru jika target lama
tidak sesuai. Konfigurasi yang sudah benar tidak ditimpa pada build ulang.

Kalau OLED Anda **SH1106**, alamat I2C berbeda, atau tombol active-high,
jalankan dari root repo untuk mengedit konfigurasi **yang sama**:
```powershell
python "$env:IDF_PATH\tools\idf.py" -C firmware -B .buildtmp\idf-esp32s3 -D IDF_TARGET=esp32s3 -D "SDKCONFIG=$PWD\.buildtmp\sdkconfig-esp32s3" menuconfig
```
Masuk **"Jalak local OLED demo (hardware UNVERIFIED)"** dan sesuaikan:
- `GPIO2 module/button is active-low`
- `Use SH1106 instead of SSD1306`
- `OLED I2C address`

## 6. Build

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\tools\flash-windows.ps1 -NoFlash -SkipAssets
```

- **Sukses** → muncul `Project build complete` dan file
  `.buildtmp\idf-esp32s3\sijalak_bedas.bin`, `flash_args`, dan `flasher_args.json`.
  Semua image yang direferensikan harus ada dan tidak kosong; jika tidak, skrip gagal.
- **Error** → salin **baris error pertama** (bukan yang terakhir), kirim ke
  developer. Jangan lanjut flash sebelum build hijau.

## 7. Cari port COM

1. Colok board via USB.
2. Device Manager → **Ports (COM & LPT)** → catat misalnya `COM5`.
   (Atau jalankan di PowerShell: `Get-PnpDevice -Class Ports`.)

## 8. Flash & monitor

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\tools\flash-windows.ps1 -Port COM5 -Monitor -SkipAssets
```
Ganti `COM5` dengan port Anda. 
- **Keluar dari monitor:** tekan `Ctrl + ]`.

Skrip selalu build/validasi dulu. Baud default 460800; bisa diubah lewat `-Baud`.
`-NoFlash` tidak mendeteksi/mengakses COM. Pilihan `.bat` yang kosong/salah tidak
bisa memulai flash. Log: `.buildtmp\flash-windows.log`.

## 9. Apa yang seharusnya terlihat

- OLED: layar wajah. **Tap** → menu. **Double-tap** → pilih.
- Menu: pilih **Wajah** atau **Pomodoro**. **Tap** ganti pilihan,
  **double-tap** memilih.
- Pomodoro: **double-tap** mulai/pause, **tahan** reset & kembali.
- Kalau SD terpasang: wajah menampilkan animasi Mochi; tap memicu **pop sound**.
- Speaker: nada UI saat navigasi; pop saat interaksi wajah.

## 10. Kalau ada masalah

| Gejala | Kemungkinan & tindakan |
|---|---|
| `Failed to connect ... No serial data received` | Tahan **BOOT**, tekan **RST**, lepas BOOT, ulangi flash. Cek kabel data & COM. |
| OLED kosong | Cek wiring SDA8/SCL9, alamat I2C, dan tipe controller (SSD1306 vs SH1106). Periksa log monitor `OLED unavailable`. |
| Tidak ada suara | Cek wiring amp (BCLK4/LRC5/DIN6) & daya amp. Log `I2S amp unavailable`. |
| Animasi tidak muncul | SD tidak ter-mount atau folder `\frames\` salah. Log `SD unavailable`. |
| Board reset berulang | Cek `CONFIG_SPIRAM_MODE_OCT` sesuai modul; kalau PSRAM beda, ubah di menuconfig. |
| Touch tidak responsif | Cek polaritas (active-low vs high) di menuconfig; cek pin GPIO2. |

## 11. Kalau ingin pakai WSL2 (alternatif)

Kalau lebih nyaman Linux:
```powershell
wsl --install -d Ubuntu
```
Di dalam Ubuntu: `sudo apt install -y git cmake ninja-build python3 python3-venv`,
lalu clone ESP-IDF v5.4 dan `./install.sh esp32s3`, `source export.sh`.
Untuk build: `python "$IDF_PATH/tools/idf.py" -C firmware -B .buildtmp/idf-s3-wsl -D IDF_TARGET=esp32s3 build`.
**Flashing dari WSL** perlu `usbipd-win` untuk meneruskan port
serial — lebih ribet; **flashing lebih mudah dari Windows native**.

---

## Ringkasan cepat (kalau ESP-IDF sudah terpasang)

```powershell
# Dari root repo, build saja:
. "$HOME\esp\esp-idf\export.ps1"; python "$env:IDF_PATH\tools\idf.py" -C firmware -B .buildtmp\manual-s3 -D IDF_TARGET=esp32s3 build
# Flash hanya setelah verifikasi port dan build:
powershell -NoProfile -ExecutionPolicy Bypass -File .\tools\flash-windows.ps1 -Port COM5 -SkipAssets
```

## Batas yang harus dipahami

- **Polaritas touch, controller OLED, dan pin SD belum diverifikasi** untuk
  modul Anda. Verifikasi datasheet/schematic sebelum menyolder.
- Pin SD default **CS10/MOSI11/SCK12/MISO13** dipakai sesuai input owner; tetap
  cek kesesuaian modul.
- Firmware ini **demo lokal**: tanpa mic, tanpa Wi-Fi/cloud, tanpa voice. Fitur
  suara = speaker saja.
- Jangan klaim "berhasil" sebelum melihat sendiri di board.
- Source lokal masih memerlukan file yang belum tracked (misalnya
  `Kconfig.projbuild`, preview, dan boundary headers). Checkout remote belum
  lengkap sampai file tersebut ikut disertakan. Tidak ada push dalam perbaikan ini.
