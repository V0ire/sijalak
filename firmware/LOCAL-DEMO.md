# Demo OLED lokal sementara

Default firmware berisi wajah, menu dua pilihan (Wajah/Pomodoro), timer fokus
25 menit + jeda 5 menit, animasi Mochi dari microSD, dan suara (pop Mochi +
nada UI). Tidak ada mic, Wi-Fi, HTTP, cloud, atau opt-in voice. `legacy_voice.cpp`
dan driver HTTP tetap disimpan, tetapi tidak diregistrasikan dalam build default.

## Animasi dan suara dari SD

Aset GIF Watcher Mochi dan pop sound termasuk aset pihak ketiga (Dasai / "Cute
Character Sound") yang dilisensikan **untuk pemakaian pribadi** proyek tersebut.
Karena itu aset dibaca dari **microSD**, dan `tools/mochi_assets.py` mengonversi
GIF → frame OLED 1-bit + `pop.pcm`. Hasil konversi masuk ke
`firmware/preview/assets/` yang **di-ignore git**; jangan commit atau
mendistribusikannya. Repo hanya berisi kode konverter dan loader.

Format yang dibaca firmware dari kartu:
```
/sdcard/frames/<nama>.bin   frame 1-bit 128x64 (1024 byte/frame, LSB = piksel atas)
/sdcard/frames/index.json   metadata (tidak wajib dibaca firmware)
/sdcard/pop.pcm             16 kHz mono int16 PCM (pop Mochi)
```
Nama animasi yang dicoba saat boot: `happy`, `love`, `wink`, `sleepy` (yang ada
dipakai). Mic (INMP441) **dinonaktifkan** (`-1`); hanya speaker (MAX98357A) yang
dipakai.

## Preview offline yang bisa diklik

Dari root proyek, tanpa install dependensi:

```sh
export TMPDIR="$PWD/.buildtmp"
cmake -S firmware/test -B .buildtmp/local-demo-host -DCMAKE_BUILD_TYPE=Debug
cmake --build .buildtmp/local-demo-host
ctest --test-dir .buildtmp/local-demo-host --output-on-failure
python3 firmware/preview/serve.py --runner .buildtmp/local-demo-host/jalak_local_preview
```

Buka **http://127.0.0.1:8765**. Server hanya bind loopback; Ctrl+C berhenti.
Satu simulasi bersama untuk seluruh tab; restart server mereset demo.
Python mengirim level tombol dan clock monotonic ke runner C++ yang memakai
`Demo`, `Button`, dan `Framebuffer` persis seperti firmware. Browser hanya
menampilkan 1024 page bytes (LSB = piksel paling atas per halaman), bukan
menghitung timer atau menggambar karakter tersendiri. Tidak ada koneksi board.

Klik kontrol berlabel, Tab + Enter/Space, atau N/S/B. Simulasi +25/+5 menit
hanya mempercepat clock preview. Durasi gestur virtual juga memajukan clock
(tap 401 ms, double 581 ms, hold 1371 ms); ini bukan latensi board terukur.
Pilihan kurangi gerak mengikuti `prefers-reduced-motion`; timer tetap berjalan.

## GPIO2: satu input digital

- **Tap:** buka menu (pilihan awal Wajah); di menu pilih berikutnya.
- **Double tap:** pilih layar; di timer mulai/pause/resume. Setelah selesai,
  double tap memulai siklus baru 25 menit. Di wajah: kedipan singkat.
- **Hold:** ≥1000 ms setelah press stabil. Di timer: reset fokus 25:00,
  berhenti, kembali ke wajah. Di menu/wajah: kembali ke wajah **tanpa reset**.
- Timer terus berjalan ketika menu/wajah dibuka. Fokus otomatis lanjut ke
  jeda 5 menit; setelah jeda selesai berhenti (tidak berulang otomatis).
- Debounce 30 ms. Single tap ditunda sampai jendela double >300 ms setelah
  release stabil. Raw edge press kedua pada ≤300 ms termasuk double; release
  kedua harus bukan hold. Hold satu kali, termasuk input stuck; release hold
  tidak menambah tap. Pause tidak mengurangi sisa waktu.
- Semua clock/delta 64-bit unsigned. Sampel waktu mundur diabaikan seluruhnya,
  termasuk gestur, sampai clock mencapai timestamp terakhir. Delta besar
  dapat menyelesaikan satu siklus tanpa catch-up loop/overflow. Reboot/reset
  proses memulai dari wajah dengan timer berhenti 25:00; tidak ada persistensi.

## Batas perangkat dan build

Wiring owner: **SDA8/SCL9, GPIO2**, amp **BCLK4/LRC5/DIN6**, SD **CS10/MOSI11/SCK12/MISO13**.
Default SSD1306 128×64, alamat 0x3c, GPIO2 active-low semuanya **UNVERIFIED**.
`menuconfig` bagian "Jalak local OLED demo" menyediakan SH1106 (offset 2), alamat,
dan polaritas active-low. Ini GPIO dari modul sentuh ber-output digital atau
tombol, bukan pemindaian pad kapasitif native ESP32. Verifikasi tegangan/polaritas
dan wiring modul sebelum dipakai.

**Catatan pin SD:** klaim lama "GPIO11–13 = flash/PSRAM di N16R8" **keliru** —
grup SPI0/1 flash di S3 adalah GPIO26–32 (dan sebagian modul OPI 33–37). Pin SD
owner dipakai apa adanya, tetapi **wajib** dicek dari datasheet/schematic modul
Anda sebelum di-solder. Tidak ada verdict aman/bentrok yang diklaim di sini.

Display gagal saat init: tidak ada present/retry. Write gagal: stop write
berikutnya, timer/input terus berjalan tanpa audio/network. Input gagal init:
tidak dibaca. SD gagal mount: memakai mata bawaan, tanpa suara. I2S gagal init:
jalan senyap. I2C tetap sinkron dengan timeout driver 50 ms; respons input bukan
realtime atau interlock keselamatan.

**Belum target-compiled / belum flash-ready / belum hardware-tested.** Mesin
sesi ini tidak memiliki `idf.py`, `IDF_PATH`, atau compiler Xtensa S3. Host
compilation dengan boundary stubs adalah simulasi entrypoint, bukan kompilasi
HAL/SDK asli. IDF 5.4+ perlu build penuh di lingkungan yang sudah memilikinya;
periksa error pertama sebelum mempertimbangkan flash. Tidak dilakukan install,
flashing, pengukuran, atau akses perangkat di sesi ini.

Koreksi pin: klaim lama GPIO11–17 sebagai flash/PSRAM N16R8 generik salah.
Tidak ada verdict pin SD dari `sdkconfig`: mapping owner CS10/MOSI11/SCK12/MISO13
tetap disimpan di legacy dan tidak didorong. SD di luar lingkup demo. Verifikasi
datasheet/schematic varian modul tertentu, bukan asumsi label N16R8.

## Visual dan lisensi

Dua mata monochrome, blink/lihat samping/kedipan respons adalah gambar dan
logika orisinal demo ini. Inspirasi interaksi saja dari Watcher Mochi, bukan
port native SenseCAP/LVGL atau salinan GIF. Upstream README: source Apache-2.0,
GIF Dasai milik Dasai untuk personal use proyek tersebut. Tidak ada source,
font, atau aset upstream yang disalin. File karakter/animasi dan preview
workstream OLED/CAD tidak diubah.
