# Si Jalak Bedas — Discovery Lapangan

**Status: usulan, belum dilaksanakan atau disepakati dengan mitra.** Wilayah awal: Kecamatan Pameungpeuk, Kabupaten Bandung. Belum ada TBM spesifik, kerja sama, observasi, atau izin partisipasi. Dokumen ini bukan izin menghubungi pihak luar atau melakukan penelitian.

## 1. Pertanyaan yang Hendak Dijawab

- Apa yang dilakukan pengunjung sebelum memilih buku dan memulai aktivitas membaca?
- Pada bagian mana mereka berhenti, beralih aktivitas, atau meminta bantuan? Apakah mereka memang ingin membaca pada kunjungan itu?
- Apakah hambatannya dapat dibantu interaksi robot, atau lebih tepat ditangani melalui koleksi, penataan buku, dan pendampingan manusia?
- Apakah pengunjung dapat memulai interaksi sendiri dan berpindah ke buku tanpa pengelola memandu setiap langkah?
- Apa konsekuensinya bagi pengunjung lain dan beban kerja pengelola?

**Hipotesis sementara:** ajakan robot dapat membantu pengunjung mulai membaca. **Preferensi yang diriset:** pengunjung memulai sendiri. Keduanya boleh direvisi atau ditolak berdasarkan temuan.

**YOLO planning update2026-10-09:** asumsi cohort awal10–14 dan hook cerita pendek +pilihan buku terbatas dipilih untuk rencana pilot, bukan hasil observasi. Gemini Developer API tidak direncanakan untuk pilot anak karena batas18+/child-directed API clients pada ketentuan yang ditinjau. Pilot memakai profil lokal terisolasi, tanpa perekaman/egress suara atau panggilanGemini; konten human-authored/recorded/licensed. Demo D3 adalah jalur internal dewasa terpisah dengan gate kelayakan provider. `DECISIONS.md`, `MVP.md` dan `AI-SPEC.md` menjadi baseline; wawancara tetap menguji/revisi asumsi.

## 2. Informasi Lapangan sebelum Menetapkan MVP

| Informasi | Cara memperoleh | Keputusan yang dipengaruhi |
|-----------|-----------------|---------------------------|
| Tujuan kegiatan TBM, jadwal, pola kunjungan | Wawancara pengelola dan observasi | Apakah hipotesis intervensi sesuai kebutuhan lokasi |
| Kelompok pengunjung, bahasa, pilihan bacaan, kebutuhan aksesibilitas | Keterangan pengelola secara agregat dan percakapan sukarela | Segmen pilot dan bahasa interaksi; usia belum dipilih |
| Alur nyata dari datang hingga memilih/membaca buku | Observasi kegiatan normal | Titik intervensi dan definisi keberhasilan |
| Hambatan, bantuan yang sudah diberikan, dan pengecualian | Peristiwa teramati dan cerita kejadian konkret | Apakah robot memiliki nilai tambahan dibanding bantuan sederhana |
| Koleksi yang benar-benar tersedia dan mudah ditemukan | Penelusuran bersama pengelola | Kelayakan rekomendasi, kurasi, dan pengantar ke buku fisik |
| Kebisingan, posisi perangkat, lalu lintas pengunjung, antrean | Observasi dan pemetaan lokasi bersama pengelola | Audio, pemicu interaksi, penempatan, dan kebutuhan pendampingan |
| Listrik, pengisian, Wi-Fi, penyimpanan perangkat, operator | Konfirmasi pengelola; jangan diasumsikan | Daya, konektivitas, arsitektur, dan perawatan |
| Batas humor, aturan konten anak, izin, privasi | Diskusi pengelola serta prosedur partisipasi | Persona, penggunaan mikrofon, data, dan keselamatan |
| Anggaran, tenggat, jumlah unit, waktu pengelola | Pemilik proyek dan pengelola | Batas lingkup dan kelayakan pilot |

Catat sumber setiap temuan: **[O] observasi**, **[K] keterangan peserta/pengelola**, **[I] interpretasi**, **[?] belum diketahui**. Jangan mencatat pendapat pengelola atau dugaan peneliti sebagai perilaku yang sudah diamati.

## 3. Rancangan Discovery Ringan

Angka berikut adalah perkiraan untuk menyusun jadwal, bukan kuota penelitian atau target yang sudah disetujui. Sesuaikan setelah mengetahui kegiatan TBM.

Urutan ini mendasari pemilihan MVP dan pilot TBM, bukan prasyarat untuk seluruh demo teknis. Demo perangkat dalam konteks yang disepakati boleh mendahului riset lapangan lengkap hanya setelah scope dan implementasinya disetujui secara eksplisit. Demo tidak menggantikan baseline, izin partisipasi, atau validasi kebutuhan untuk pilot.

1. **Pilih calon lokasi bersama pemilik proyek.** Pastikan ada kegiatan literasi yang dapat diamati, pengelola bersedia berdiskusi, dan mekanisme izin tersedia. Jangan mengklaim calon lokasi sebagai mitra sebelum dikonfirmasi.
2. **Wawancara pengelola, sekitar 10–15 menit.** Minta contoh kejadian dan penjelasan kegiatan, bukan persetujuan terhadap solusi robot.
3. **Observasi baseline, usulan dua periode 30–45 menit pada kegiatan berbeda.** Amati kegiatan biasa tanpa robot atau perubahan penataan terlebih dahulu. Catat konteks karena jenis kegiatan dan kehadiran relawan dapat mengubah perilaku.
4. **Percakapan sukarela dengan calon pengguna, sekitar 3–5 menit per orang.** Contoh awal 4–6 peserta yang menunjukkan pola berbeda: memilih sendiri, perlu bantuan, atau memilih aktivitas lain. Ini eksplorasi, bukan sampel representatif; jangan memaksakan kuota usia sebelum mengetahui pengunjung.
5. **Sintesis singkat bersama pengelola.** Pisahkan kejadian, dugaan penyebab, dan kebutuhan. Periksa pula pengunjung yang sudah mulai membaca tanpa bantuan.
6. **Jika ada dasar dan izin, bandingkan konsep interaksi.** Gunakan penjelasan atau simulasi terbuka sebelum memfinalkan MVP/pilot TBM. Beri tahu peserta bila respons diatur fasilitator; jangan menyamarkan simulasi sebagai AI otonom.

### Izin dan Batas Partisipasi

- Sebelum meminta persetujuan, jelaskan tujuan, kegiatan, perkiraan durasi, dan catatan anonim yang akan dibuat dengan bahasa yang dipahami peserta. Jelaskan bahwa ikut bersifat sukarela dan boleh berhenti tanpa memengaruhi akses TBM; beri kesempatan bertanya dan periksa pemahaman.
- Sepakati izin lokasi, penanggung jawab, dan tata cara partisipasi sebelum kegiatan. Untuk wawancara atau uji konsep langsung dengan anak, pastikan persetujuan wali melalui prosedur yang disepakati dan kesediaan anak; izin pengelola saja tidak dianggap menggantikannya.
- Penolakan atau berhenti tidak mengurangi hak memakai TBM. Hindari hadiah yang membuat peserta merasa wajib ikut atau membaca.
- Jangan mengumpulkan nama, kontak, identitas sekolah, foto, rekaman suara, atau profil individual lintas kunjungan pada discovery awal. Gunakan hitungan agregat atau kode sesi sementara tanpa identitas.
- Hindari kutipan yang dapat mengidentifikasi anak. Tetapkan akses dan masa simpan catatan sebelum pengumpulan; keduanya belum ditentukan.
- Jangan melakukan diagnosis kemampuan baca, mempermalukan peserta, atau menguji sarkasme yang menyerang anak. Pengunjung harus tetap bisa meminta bantuan manusia.
- Observasi tanpa robot tidak berarti observasi diam-diam. Ikuti prosedur pemberitahuan dan izin yang disepakati, termasuk pilihan tidak berpartisipasi.

## 4. Panduan Wawancara Pengelola

Pilih pertanyaan sesuai percakapan; tidak harus menanyakan semuanya.

1. “Ceritakan kegiatan terakhir di sini. Pengunjung biasanya melakukan apa setelah datang?”
2. “Pada kegiatan itu, siapa yang memilih buku sendiri, dan kapan ada yang meminta bantuan? Bisa beri contoh tanpa menyebut identitas?”
3. “Pernah ada pengunjung yang tidak jadi membaca? Apa yang Anda lihat terjadi, dan apa yang masih dugaan?”
4. “Apa yang sudah dicoba untuk mengajak membaca? Kapan berhasil dan kapan tidak?”
5. “Kalau pengunjung diarahkan ke satu buku, bagaimana memastikan bukunya tersedia dan mudah ditemukan?”
6. “Kapan pengelola bisa membantu perangkat, dan kapan sedang sibuk? Apa batas suara, pengisian, penyimpanan, atau pengawasan di sini?”

Jangan membuka dengan “Apakah robot akan meningkatkan minat baca?” Jawaban persetujuan tidak menunjukkan hambatan atau manfaat nyata.

## 5. Panduan Percakapan Calon Pengguna

Gunakan bahasa sesuai peserta, tanpa menguji atau memaksa jawaban.

- “Tadi datang ingin melakukan apa?”
- “Biasanya memilih buku dari mana? Boleh tunjukkan, kalau mau?”
- “Ceritakan terakhir kali ingin membaca di sini. Apa yang terjadi?”
- Jika peserta menyebut kesulitan: “Bagian mana yang terasa susah? Biasanya minta bantuan siapa?”
- “Apa yang membuatmu mau mulai melihat atau membaca buku ini?”

Untuk uji konsep nanti: minta peserta menjelaskan dengan kata sendiri apa yang harus dilakukan setelah ajakan. Amati apakah mereka menemukan buku dan memulai aktivitas, bukan hanya menjawab bahwa konsepnya lucu. Tanyakan bagian membingungkan atau mengganggu secara netral.

## 6. Catatan Observasi dan Definisi Awal

Catat per kesempatan yang diamati: jenis kegiatan, bantuan relawan, akses buku, pemilihan buku, waktu sampai mulai, perpindahan aktivitas, dan bantuan tambahan. Bedakan **melihat/memegang buku**, **keterlibatan dengan isi**, **membaca dengan bantuan**, dan **bermain dengan robot**.

Definisi “mulai membaca” harus disepakati dengan pengelola setelah segmen diketahui. Usulan operasional untuk diuji: keterlibatan sukarela dengan isi buku >=30detik atau mulai membaca dengan bantuan; bukan hanya memegang/membuka buku. Angka dan cohort awal adalah asumsi, dapat direvisi sebelum pilot. Keterlibatan dengan buku bergambar atau membaca bersama dapat relevan; jangan menjadikan membaca teks secara mandiri sebagai satu-satunya bentuk literasi. Pengamatan perilaku juga tidak membuktikan pemahaman atau peningkatan kemampuan baca.

Usulan ukuran untuk uji berikutnya:

- **Utama:** proporsi kesempatan yang berakhir dengan mulai membaca menurut definisi yang disepakati; laporkan pembilang, penyebut, dan bantuan manusia.
- **Pendukung:** waktu dari ajakan hingga mulai, keberhasilan menemukan buku, langkah yang membingungkan, serta permintaan bantuan.
- **Penjaga:** gangguan bagi pembaca lain, antrean, waktu fasilitasi pengelola, ketidaknyamanan, dan pengunjung yang hanya bermain dengan robot.

Jangan membandingkan seluruh pengunjung baseline dengan hanya pengguna robot yang sukarela mendekat. Gunakan kesempatan/ajakan yang sebanding, catat siapa yang memilih ikut, dan jangan mengklaim kausalitas dari sampel kecil.

## 7. Dasar untuk Melanjutkan ke Pilihan MVP

- Ada kebutuhan atau hambatan konkret, termasuk temuan yang bertentangan dengan hipotesis awal.
- Segmen pilot dipilih berdasarkan relevansi kebutuhan dan akses, bukan sekadar umur yang diasumsikan cocok.
- Ada alur yang dapat diuji dari ajakan ke buku fisik yang tersedia.
- Pengelola dapat mendukung penggunaan dan prosedur izin/keamanan.
- Definisi perilaku serta pembanding disepakati; target angka belum ditetapkan.
- Bandingkan nilai tambahan robot dengan ajakan relawan, kurasi/penataan buku, atau media sederhana. Jika alternatif tanpa robot memadai, catat itu sebagai hasil yang sah.

**Berikutnya:** temuan lapangan menguji baseline sementara dalam `REQUIREMENTS.md`, `MVP.md` dan `BLUEPRINT.md`. Pilihan planning bukan fakta tervalidasi dan dokumen ini tidak mengizinkan observasi, kontak, atau implementasi.
