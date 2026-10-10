# Si Jalak Bedas — Project

**Status: PLANNING COMPLETE — NOT IMPLEMENTED.**37requirements,8fase,11rencana; eksekusi0%. Parser/static/reviewMAIN selesai. Independent nativechecker unavailable akibat usage limit, bukan diklaim lulus; lihat `PLANNING-VERIFICATION.md`.
**Diperbarui: 2026-10-09.** Baseline terpilih untuk perencanaan, belum diimplementasikan atau divalidasi lapangan.

## Produk dan nilai inti
Si Jalak Bedas adalah robot kecil berkarakter ekspresif untuk kegiatan literasi di TBM/perpustakaan. Nama dan persona masih dapat direvisi.
Nilai inti: membantu pengunjung mulai membaca, lalu menyerahkan perhatian ke buku. Ini hipotesis intervensi, bukan akar masalah atau dampak yang terbukti.
Belum ada observasi pengunjung, mitra TBM, hasil pilot, atau bukti bahwa robot lebih bermanfaat daripada ajakan sederhana.

## Arahan pengguna (U)
- Wilayah awal: Kecamatan Pameungpeuk, Kabupaten Bandung; belum ada TBM spesifik atau kerja sama.
- Pengunjung diharapkan memulai sendiri; pengelola tersedia bila dibutuhkan. Ini preferensi, bukan penggunaan tanpa pengawasan yang tervalidasi.
- Elektronik awal dimiliki: ESP32-S3 N16R8, OLED 1,3 inci 128×64, INMP441, MAX98357A/speaker 3W, dan microSD.
- LiPo 3,7V sekitar 450 mAh serta modul pengisian/konversi juga dilaporkan dimiliki; varian, kondisi, kompatibilitas, dan keselamatan belum diperiksa.
- Tambahan komponen/daya/casing satu prototipe maksimal Rp500.000; tidak mencakup elektronik milik pengguna, printer, riset/pilot, atau biaya operasional VPS/API. Bukan izin belanja.
- Milestone pertama: demo fungsional D3, percakapan AI fleksibel dalam konteks terbatas; keberhasilan demo tidak membuktikan manfaat literasi.
- Robot memakai VPS melalui API tanpa laptop/PC/ponsel pendamping operasional.
- VPS/API dan tahap STT, chat, TTS dilaporkan tersedia; belum diakses atau diuji.
- Laporan terbaru “Gemini, presumably” tetap identifikasi penyedia yang tidak pasti, bukan bukti akun/model siap.
- Baseline bentuk pengguna: tinggi 140–160 mm dan konsumsi PLA sekitar 130–170 g; printer Adventurer 5-series kemungkinan 5M, belum dikonfirmasi.

## Baseline otomatis (A)
[DECISIONS.md](DECISIONS.md) memuat pilihan, alasan, alternatif, dan pemicu revisi. U = laporan pengguna; D = dokumentasi; A = asumsi terpilih; V = validasi tertunda.
- Dua keluaran terpisah: D3 internal adult-lab 18+ dan pilot child-local; pemisahan ini pilihan planning karena gate penyedia.
- Cohort pilot sementara 10–14 tahun; bukan demografi lapangan atau kebutuhan baca yang sudah diketahui.
- MVP literasi sementara: hook buku singkat terkurasi ditambah pilihan terbatas, dibandingkan ajakan/pilihan/hook dan pembanding tanpa robot.
- Demo half-duplex push-to-talk (PTT) dengan STOP terpisah; maksimal 4 turn/180 detik per sesi, tanpa always-listening.
- Input WAV PCM16 mono 16 kHz maksimal 8 detik; output WAV PCM16 mono 24 kHz maksimal 15 detik; respons maksimal 35 kata.
- Firmware ESP-IDF 6.1.x; gateway async FastAPI/Python 3.12 dengan adapter STT→chat→TTS. Versi patch/dependensi belum dipin atau dipasang.
- REST job/polling dipilih; hostname `UNKNOWN` dan namespace `/sijalak/v1` hanya kontrak usulan, bukan alamat/rute aktif VPS.
- ID model/voice dalam [AI-SPEC.md](AI-SPEC.md) provisional; dukungan akun, kontrak, harga, dan kualitas belum dibuktikan.
- Bahasa Indonesia; humor diarahkan ke robot/situasi, tidak mempermalukan kemampuan baca atau identitas pengguna.
- Demo awal direncanakan memakai suplai eksternal terregulasi yang kelak diverifikasi; runtime/proteksi/power-path baterai tidak diasumsikan aman.
- Profil/build dan izin backend adult-lab dipisahkan dari child-local; kontrol pengguna tidak boleh membuka rute AI pada profil anak.

## Industrial Design, enclosure, dan karakter
- Konsep utama original hybrid **Jalak Gembul**; tiga konsep dibandingkan di [INDUSTRIAL-DESIGN.md](INDUSTRIAL-DESIGN.md).
- Tinggi nominal 150 mm; torso 112×102×122 mm (W×D×H). Semua masih parameter desain.
- Empat bagian casing: body/torso, crown, base/service cap, dan visor/bezel; bukan model yang sudah dibuat.
- Reservasi central clear 70×55×80 mm adalah syarat desain, bukan bukti semua komponen muat atau dapat diservis.
- Area aktif OLED sekitar 30×15 mm hanya referensi modul; dimensi aktual milik pengguna belum diukur, visor tidak memperbesar layar.
- Target total konsumsi PLA 130–170 g, cap 170 g termasuk seluruh part, support, brim, purge/limbah; bukan massa rakitan. Lebih ringan boleh jika fit/kualitas terjaga.
- Asumsi printer 5M ber-volume 220³ mm belum memvalidasi printer milik pengguna, akses, nozzle, filament, atau biaya cetak.
- Layout/mounting PCB, koordinat komponen, dan wiring detail ditunda; fit, slicing, stabilitas, panas, dan akustik belum diuji.

## Gate provider dan penggunaan anak
Snapshot live yang tercatat di [AI-SPEC.md](AI-SPEC.md): ketentuan Gemini Developer API bertanggal 2026-04-28 mewajibkan pengguna 18+ dan melarang API client yang ditujukan kepada atau kemungkinan diakses usia di bawah 18.
VPS, proxy dewasa, orang tua/persetujuan wali, atau tier berbayar tidak membypass pembatasan tersebut; jangan meneruskan ucapan anak melalui orang dewasa.
D3 hanya direncanakan sebagai client R&D internal dewasa yang terpisah dan terbatas; eligibility penawaran/akun/kontrak wajib diperiksa sebelum penggunaan live.
Pilot anak memakai hook manusia/orisinal, rekaman manusia, atau konten berlisensi yang tersedia lokal; tanpa penangkapan audio atau cloud AI, termasuk pipeline Gemini untuk membuat skrip anak.
AI live pada produk anak tetap terblokir sampai ada penyedia/kontrak yang eligible. Pilot lokal tetap bergantung bukti lapangan, izin, dan kesiapan perangkat.

## Otorisasi dan larangan
- **YOLO ALL PLANNING:** seluruh perencanaan otomatis tanpa pertanyaan, termasuk `REQUIREMENTS.md`, `ROADMAP.md`, dan `STATE.md`; menggantikan larangan pembuatan artefak tersebut sebelumnya.
- Perubahan saat ini dibatasi artefak planning dan pengaturan planning lokal; agen hanya menulis path planning yang ditugaskan, bukan kode produksi atau konfigurasi global.
- Tidak ada izin implementasi firmware/backend, pembuatan kode/model/aset, CAD/STL/G-code, slicing, pencetakan, atau pengujian perangkat/lapangan.
- Tidak menjalankan installer atau stock GSD CLI/workflow yang memutasi Git/menyalakan eksekusi. Perhitungan geometri dan parser/check dokumen read-only serta riset dokumentasi publik diperbolehkan; bukan tes perangkat/aplikasi atau akses VPS.
- Tidak memanggil API, mengakses/mengubah VPS, deploy, memakai kredensial, mengaktifkan billing, atau membeli perangkat/layanan.
- Tidak melakukan Git/init/commit/tag atau mengubah konfigurasi global OpenCode, Pro, 9Router, plugin, maupun integrasi yang ada.
- Tidak menghubungi TBM/pengelola/peserta, melakukan observasi, atau menyelenggarakan pilot.
- Eksekusi mendatang memerlukan checkpoint izin implementasi dan izin eksternal yang relevan; izin planning tidak meloloskan checkpoint tersebut.

## Unknowns yang dipertahankan
- Mitra, izin, pengunjung aktual, kemampuan/bahasa baca, aksesibilitas, hambatan, koleksi buku, dan beban pengelola.
- Varian/envelope komponen, controller OLED, kontrol fisik, suplai/proteksi, runtime, fit, dan akses produksi casing.
- Hostname/schema API nyata, lokasi pemrosesan tiap tahap, identitas penawaran/akun/model, retensi, kapasitas, kuota, harga, dan latensi.
- Harga tambahan, akses/printer, biaya total dan operasional; alokasi biaya di DECISIONS adalah asumsi, bukan penawaran atau izin pengeluaran.
- Tenggat/audiens/lokasi demo, kondisi jaringan/audio, definisi dan baseline membaca, target manfaat, serta durasi pilot.
- Efektivitas hook/pilihan dan kesesuaian persona; segmen atau fitur baseline dapat direvisi setelah bukti tersedia.

## Workflow perencanaan
1. Konsolidasikan sumber kanonis di bawah; beri label laporan pengguna, dokumentasi, asumsi, dan validasi tertunda.
2. Alur planning yang diizinkan memakai agen native harness `gsd-planner` untuk rencana dependency-aware, tanpa pertanyaan tambahan.
3. Agen native `gsd-plan-checker` dipanggil namun gagalusage limit, tanpa verdict. Ulangi ketika tersedia sebelum implementasi.
4. MAIN memeriksa parser/struktur/coverage/dependensi/sintaks dan konsistensi, memperbaiki temuan dan menutup planning sementara dengan gap checker yang dinyatakan jujur.
Jangan memakai stock `new-project`, auto-chain, CLI/skrip, atau workflow yang melakukan Git/eksekusi implisit. Gunakan planner/checker native langsung dengan override planning-only dan tanpa init/hooks/commit.
Verdict planner/checker menilai mutu rencana; bukan PASS hardware, API, safety, atau manfaat literasi.

## Dokumen kanonis
| Dokumen | Fungsi |
|---------|--------|
| [DECISIONS](DECISIONS.md) | Provenance, pilihan terpilih, trade-off, dan revision gates |
| [REQUIREMENTS](REQUIREMENTS.md) | Scope dan traceability; belum implemented/validated |
| [ROADMAP](ROADMAP.md) | Delapan fase mendatang, dependensi, dan checkpoint izin |
| [BLUEPRINT](BLUEPRINT.md) | Outline produk dan indeks capability/alur/arsitektur/verifikasi |
| [ARCHITECTURE](ARCHITECTURE.md), [AI-SPEC](AI-SPEC.md) | Kontrak teknis usulan serta eligibility/data/eval AI |
| [MVP](MVP.md) | Batas demo dewasa dan MVP literasi lokal bersyarat |
| [INDUSTRIAL-DESIGN](INDUSTRIAL-DESIGN.md) | Perbandingan konsep, envelope, assembly, material, dan validasi cetak |
| [DISCOVERY](DISCOVERY.md), [USER-FLOWS](USER-FLOWS.md), [VERIFICATION-STRATEGY](VERIFICATION-STRATEGY.md) | Rencana lapangan, interaksi, dan bukti penerimaan |

Tidak ada kode aplikasi/model/aset produksi atau hasil tes perangkat/API/lapangan yang tersedia dari perencanaan ini. Pemeriksaan parser/static/review planning dilaporkan terpisah; jangan mengisi unknowns dengan bukti rekaan.
