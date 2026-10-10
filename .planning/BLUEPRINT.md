# Si Jalak Bedas — Blueprint

**Status: PLANNING COMPLETE — NOT IMPLEMENTED.**Parser/static/reviewMAIN selesai; independent nativechecker unavailableusage limit, bukan diklaimPASS. Tidak ada kode aplikasi/modelCAD/aset, tes perangkat/API atau hasil lapangan.
Outline ini mengindeks baseline terpilih di dokumen kanonis; kontrak rinci dan alasan keputusan tetap berada di dokumen pemiliknya.

## 1. Kapabilitas dan scope
| Capability | Fitur/subfitur baseline | Rujukan |
|------------|------------------------|---------|
| D3 adult-lab | Percakapan fleksibel terbatas; PTT, status OLED, suara robot→VPS STT/chat/TTS→speaker, STOP/reset | [MVP](MVP.md), [AI-SPEC](AI-SPEC.md) |
| Child-local book starter | Hook manusia/rekaman manusia/berlisensi, pilihan buku terkurasi, petunjuk buku/rak, handoff tenang | [MVP](MVP.md), [USER-FLOWS](USER-FLOWS.md) |
| Karakter dan kontrol | Mata sesuai keadaan, bahasa singkat, humor tidak merendahkan, aktivasi sukarela, error/bantuan | [USER-FLOWS](USER-FLOWS.md), [INDUSTRIAL-DESIGN](INDUSTRIAL-DESIGN.md) |
| Pemulihan dan privasi | Pembatalan, isolasi sesi, tolak respons stale, mic tidak aktif saat idle/playback/stop atau profil anak | [ARCHITECTURE](ARCHITECTURE.md), [AI-SPEC](AI-SPEC.md) |
| Operasi dan service | Kurasi/update konten, kontrol volume, stop, pemeriksaan daya, akses casing, maintainer | [REQUIREMENTS](REQUIREMENTS.md), [ROADMAP](ROADMAP.md) |
| Evaluasi literasi | Baseline, pembanding tanpa robot, transisi membaca, bantuan/gangguan/beban operator | [DISCOVERY](DISCOVERY.md), [VERIFICATION-STRATEGY](VERIFICATION-STRATEGY.md) |

Pilihan pilot adalah hook cerita dengan pilihan terbatas (C + sebagian B); A/B/C tetap pembanding yang dapat mengubah baseline setelah evidence lapangan.
Timer/reward, kamera/presence sensing, always-listening, locomotion, profil/leaderboard, RAG/web agent, dan voice cloning ditunda; bukan kebutuhan v0.1.

## 2. Indeks dokumen kanonis
| Dokumen | Isi yang dirujuk |
|---------|------------------|
| [PROJECT.md](PROJECT.md) | Nilai inti, arahan pengguna, baseline otomatis, unknowns, izin, dan workflow |
| [DECISIONS.md](DECISIONS.md) | U/D/A/V, trade-off, pilihan terpilih, revision triggers |
| [REQUIREMENTS.md](REQUIREMENTS.md) | Requirement IDs, scope, status pending, dan pemetaan fase |
| [ROADMAP.md](ROADMAP.md) | Delapan fase/sebelas rencana, dependency graph, checkpoint, dan progres eksekusi |
| [MVP.md](MVP.md) | Dua deliverable, prioritas fitur, pembanding, budget, dan acceptance usulan |
| [USER-FLOWS.md](USER-FLOWS.md) | Start/stop/handoff, keadaan robot, operator, dan pemulihan |
| [ARCHITECTURE.md](ARCHITECTURE.md) | Layer, stack, kontrak audio/job usulan, auth, data, power, dan failure behavior |
| [AI-SPEC.md](AI-SPEC.md) | Eligibility Gemini, model/voice provisional, persona/grounding, guardrails, eval, biaya |
| [INDUSTRIAL-DESIGN.md](INDUSTRIAL-DESIGN.md) | Tiga konsep, Jalak Gembul, envelope, empat part, PLA/FDM, risiko, dan gate fit/cetak |
| [DISCOVERY.md](DISCOVERY.md) | Usulan observasi/wawancara/baseline; tidak menyatakan kegiatan lapangan telah terjadi |
| [VERIFICATION-STRATEGY.md](VERIFICATION-STRATEGY.md) | Bukti dan pemeriksaan penerimaan teknis, desain, serta pilot |

Dokumentasi publik tidak membuktikan keadaan hardware/akun pemilik. Laporan pemilik tidak menjadi hasil uji; asumsi tetap diberi gate validasi.

## 3. Alur inti
Preferensi lintas mode: pengguna mendekati dan memulai sendiri, boleh mengabaikan/berhenti; pengelola tersedia. Usability belum tervalidasi.
**D3 internal dewasa:** hanya client R&D 18+ yang terpisah dan eligible sebelum live use, tanpa pendamping operasional laptop/ponsel.
1. READY → PTT eksplisit → LISTENING; pelepasan kontrol/batas input mengakhiri capture.
2. Robot mengirim audio terbatas ke gateway; STT → dialog → validasi respons → TTS, dengan status PROCESSING.
3. Robot menerima audio yang dinormalisasi dan sesuai sesi; SPEAKING tanpa capture mic bersamaan.
4. Respons mengarah ke percakapan singkat/handoff/akhir, lalu READY; batas turn/waktu/biaya mengakhiri sesi.
5. STOP/reset dari keadaan aktif membatalkan sesi, menghapus state sementara, dan menolak hasil lama; tidak menjanjikan penghapusan provider.

**Child-local:** cohort sementara 10–14, tanpa penangkapan audio, rute Gemini, atau cloud AI.
1. Aktivasi eksplisit memulai ajakan lokal dengan keadaan OLED yang dapat dipahami.
2. Hook terkurasi dan pilihan terbatas menunjuk buku yang benar-benar tersedia; tidak mengarang koleksi/rak.
3. Pengguna menuju buku; robot memberi ruang untuk membaca, bukan loop konten atau poin.
4. STOP, kebingungan, konten hilang, atau masalah perangkat mengakhiri alur/meminta bantuan pengelola.
Fallback harus jelas sebagai konten lokal/error; rekaman atau simulasi tidak dihitung sebagai keberhasilan AI.

## 4. Arsitektur dan dependensi
Baseline teknis: ESP32-S3/ESP-IDF 6.1.x menangani kontrol/OLED/audio; FastAPI/Python 3.12 di VPS mengorkestrasi adapter provider. Rincian di [ARCHITECTURE](ARCHITECTURE.md).
VPS/API serta STT/chat/TTS hanya dilaporkan tersedia. “Gemini, presumably” dan ID model tetap tidak terverifikasi; tidak ada asumsi GPU/model lokal VPS.
Hostname `UNKNOWN` dan namespace `/sijalak/v1` adalah placeholder kontrak usulan, bukan gateway/rute aktif atau dokumentasi API yang sudah diperiksa.
Audio/batas sesi, polling, auth, cancel, kuota, dan metadata mengikuti dokumen kontrak; tidak disalin ulang menjadi spesifikasi paralel di sini.
Child-local memakai build/profil dan izin backend terisolasi; tidak dapat membuka AI melalui toggle usia, parent proxy, tier berbayar, atau kontrol pengguna.
Workstream **Industrial Design, 3D Printable Enclosure & Character Design** memakai original hybrid Jalak Gembul, bukan template/mesh referensi yang disalin.
Dimensi/reservasi/PLA di [INDUSTRIAL-DESIGN](INDUSTRIAL-DESIGN.md) adalah target desain, bukan bukti fit, massa rakitan, atau printability; asumsi printer belum tervalidasi.
Tidak ada layout PCB, wiring detail, CAD/STL/G-code, slice, atau model/aset yang dibuat pada tahap ini.

## 5. MVP dan fase mendatang
[MVP.md](MVP.md) memisahkan demo teknis dewasa dari MVP literasi lokal bersyarat. Demo tidak mengunci kebutuhan produk anak.
| Fase | Keluaran yang direncanakan | Dependensi |
|------|---------------------------|------------|
| 1. Baseline Contracts and Safety | Inventaris/kontrak, provenance, biaya, safety, dan izin | Awal |
| 2. TBM Discovery and Curation | Mitra/izin, baseline, cohort, katalog nyata | 1 + izin lapangan |
| 3. Industrial Design and Printable Enclosure | Envelope/karakter, rencana CAD/fit/slice/cetak | 1 + izin terkait |
| 4. VPS Speech Gateway and AI Guardrails | Adapter dewasa, auth, batas/data/eval | 1 + eligibility/akses |
| 5. ESP32 Voice and Expression | Tracer suara/OLED/kontrol, child-local terisolasi | 1, 4 |
| 6. Integrated Prototype and Adult Demo | Integrasi fisik dan demo suara AI nyata | 3, 5 |
| 7. Literacy Pilot and Evaluation | Pilot lokal, pembanding, bantuan/gangguan/beban | 2, 6 + gate pilot |
| 8. Iteration and Handover | Revisi berbukti dan paket operator | 7 |

Graf dependensi: `1→{2,3,4}; 4→5; {3,5}→6; {2,6}→7→8`. Rincian rencana/progres hanya di [ROADMAP](ROADMAP.md).
Discovery lengkap tidak memblokir demo internal; pilot wajib menunggu bukti lapangan, izin, kesiapan fisik, dan profil lokal yang diverifikasi.

## 6. Verifikasi dan gate
- **Planning:** MAIN memeriksa konsistensi indeks, keputusan/kontrak, coverage requirement/fase, dependensi, unknowns, dan checkpoint. Review rencana bukan bukti implementasi.
- **Eligibility:** ketentuan Gemini Developer API versi 2026-04-28 mensyaratkan 18+ dan client bukan untuk/kemungkinan diakses under-18; review client dewasa tetap wajib. Tidak ada bypass VPS/proxy/wali/paid.
- **Integrasi dewasa:** bukti harus mencakup mikrofon robot, pemrosesan AI nyata melalui VPS, dan speaker robot; uji komputer saja dilabeli parsial.
- **Fault/data/biaya:** uji negatif, reset/stale/cross-session, outage, timeout, payload/format, kuota, retensi, serta truthful fallback memakai acceptance di dokumen pemiliknya.
- **Fisik:** modul/fit/service, jumlah PLA seluruh part+waste, printer/profile, stabilitas, panas, suplai, stop, keterbacaan OLED, dan akustik kelak diperiksa sebelum peserta.
- **Pilot anak:** mitra/akses/consent/willingness, buku/konten berizin, child-local tanpa audio capture/cloud AI, isolasi jaringan, dan kesiapan perangkat wajib dibuktikan.
- **Literasi:** laporkan reading-start proxy dengan denominator, kondisi pembanding, bantuan, gangguan, dan beban operator; tidak menjanjikan peningkatan pemahaman/kebiasaan.
- **Bukti:** target dan checklist masih usulan/pending; tidak ada dataset, angka hasil, PASS, atau fakta lapangan yang dibuat-buat.

## 7. Status dan langkah berikutnya
Paket planning/11nativePLANs selesai; hasil/gap checker ada di `PLANNING-VERIFICATION.md`. Berikutnya review pra-implementasi dan otorisasi terpisah, bukan API/eksekusi dari “continue”.
Semua fase eksekusi belum dimulai. Izin YOLO planning tidak memberi izin coding/install/global change/Git/commit/CAD/cetak/API/VPS/kontak/fieldwork.
Unknowns aktual dan revision gates mengikuti [PROJECT](PROJECT.md) serta [DECISIONS](DECISIONS.md); pilihan otomatis tidak ditulis ulang sebagai semuanya belum diputuskan.
**Diperbarui: 2026-10-09.**Planning selesai dengan gap checker yang dilaporkan; produk belum diimplementasikan atau divalidasi.
