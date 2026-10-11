# Panduan Flash ke ESP32-S3 di Windows

Tujuan: dari nol (belum ada ESP-IDF) sampai firmware Si Jalak demo berjalan di
board. Ikuti berurutan. Perintah **PowerShell** kecuali disebut lain.

> **Penting (jujur):** Firmware ini **belum pernah di-build dengan toolchain
> ESP-IDF asli** dan **belum pernah diuji di board**. Kode sudah lulus host test
> (ctest 12/12) tetapi itu bukan bukti hardware. Kalau `idf.py build` error,
> itu **wajar** — kirim pesan error pertama ke developer untuk diperbaiki.

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

## 5. Set target & (opsional) konfigurasi

```powershell
cd firmware
idf.py set-target esp32s3
```

Kalau OLED Anda **SH1106** (bukan SSD1306), atau alamat I2C bukan `0x3c`, atau
tombol active-high — ubah lewat menuconfig:
```powershell
idf.py menuconfig
```
Masuk **"Jalak local OLED demo (hardware UNVERIFIED)"** dan sesuaikan:
- `GPIO2 module/button is active-low`
- `Use SH1106 instead of SSD1306`
- `OLED I2C address`

## 6. Build

```powershell
idf.py build
```

- **Sukses** → muncul `Project build complete` dan file
  `firmware\build\sijalak_bedas.bin`.
- **Error** → salin **baris error pertama** (bukan yang terakhir), kirim ke
  developer. Jangan lanjut flash sebelum build hijau.

## 7. Cari port COM

1. Colok board via USB.
2. Device Manager → **Ports (COM & LPT)** → catat misalnya `COM5`.
   (Atau jalankan di PowerShell: `Get-PnpDevice -Class Ports`.)

## 8. Flash & monitor

```powershell
idf.py -p COM5 flash monitor
```
Ganti `COM5` dengan port Anda. 
- **Keluar dari monitor:** tekan `Ctrl + ]`.

Parameter flash default (16 MB board): `idf.py -p COM5 -b 460800 flash`.

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
| OLED kosong | Cek wiring SDA8/SCL9, alamat I2C, dan tipe controller (SSD1306 vs SH1106). Cek `idf.py monitor` untuk log `OLED unavailable`. |
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
lalu clone ESP-IDF v5.4 dan `./install.sh esp32s3`, `source export.sh`, lanjut
dari langkah 5. **Flashing dari WSL** perlu `usbipd-win` untuk meneruskan port
serial — lebih ribet; **flashing lebih mudah dari Windows native**.

---

## Ringkasan cepat (kalau ESP-IDF sudah terpasang)

```powershell
cd sijalak\firmware
idf.py set-target esp32s3
idf.py build
idf.py -p COM5 flash monitor
```

## Batas yang harus dipahami

- **Polaritas touch, controller OLED, dan pin SD belum diverifikasi** untuk
  modul Anda. Verifikasi datasheet/schematic sebelum menyolder.
- Pin SD default **CS10/MOSI11/SCK12/MISO13** dipakai sesuai input owner; tetap
  cek kesesuaian modul.
- Firmware ini **demo lokal**: tanpa mic, tanpa Wi-Fi/cloud, tanpa voice. Fitur
  suara = speaker saja.
- Jangan klaim "berhasil" sebelum melihat sendiri di board.
