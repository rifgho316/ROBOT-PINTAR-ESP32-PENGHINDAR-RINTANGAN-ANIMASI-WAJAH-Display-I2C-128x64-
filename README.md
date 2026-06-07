# Robot Penghindar Rintangan ESP32

Proyek ini berisi kode sumber untuk robot beroda penghindar rintangan (*obstacle-avoiding robot*) otonom. Robot ini dirancang untuk berjalan maju dan secara otomatis menghindari halangan di sekitarnya, dilengkapi dengan layar OLED yang menampilkan animasi ekspresi wajah mulus tanpa mengganggu performa pembacaan sensor.

## 🌟 Fitur Utama
* **Animasi Fisika Real-Time:** Menampilkan berbagai siklus ekspresi (Bahagia, Berkedip, Kaget, Malu/Tersipu, Marah, dan Tertawa melompat) menggunakan perhitungan matematika trigonometri tanpa jeda kaku.
* **Penghindar Rintangan Cerdas:** Menggunakan sensor ultrasonik dan motor servo untuk mengecek jarak dari jalur kiri dan kanan saat arah jalan di depannya terhalang.
* **Non-Blocking Sensor:** Animasi diproses menggunakan fungsi `millis()` (bukan `delay()`), sehingga sensor tetap responsif dan robot bisa mengerem instan saat ada halangan mendadak.
* **Kompensasi Motor:** Terdapat variabel kecepatan mandiri untuk roda kiri dan kanan agar robot berjalan lurus sempurna.

## 🛠️ Komponen yang Dibutuhkan
1. Mikrokontroler ESP32
2. 1x Layar OLED 0.96" (SSD1306)
3. 1x Sensor Ultrasonik (HC-SR04)
4. 1x Motor Servo (contoh: Micro Servo SG90)
5. 1x Motor Driver (contoh: L298N)
6. 2x Motor DC beserta Roda
7. Baterai / Power Supply
8. Sasis Robot

## 📌 Konfigurasi Pin

| Komponen | Nama Pin | Pin ESP32 | Keterangan |
| :--- | :--- | :--- | :--- |
| **OLED SSD1306** | SDA | `21` | Komunikasi I2C Default |
| | SCL | `22` | Komunikasi I2C Default |
| **Sensor Ultrasonik**| TRIG | `12` | Output Trigger |
| | ECHO | `13` | Input Echo |
| **Servo Leher** | SIGNAL | `14` | Output PWM Servo |
| **Motor Kiri** | ENA | `4` | PWM Speed Control |
| | MAJU | `16` | Direction 1 |
| | MUNDUR | `17` | Direction 2 |
| **Motor Kanan** | ENB | `19` | PWM Speed Control |
| | MAJU | `5` | Direction 3 |
| | MUNDUR | `18` | Direction 4 |

## ⚙️ Cara Penyesuaian (Tuning)
* `KECEPATAN_JALAN_KIRI = 69;` & `KECEPATAN_JALAN_KANAN = 60;` (Ubah nilai ini jika robot berbelok sendiri saat seharusnya berjalan lurus).
* Batas rintangan: Pada baris `if (jarakDepan > 0 && jarakDepan <= 20)`, ubah angka `20` menjadi lebih besar jika kamu ingin robot merespons halangan dari jarak yang lebih jauh.
* `intervalAnimasi = 10000;` (Durasi tunggu untuk berganti animasi siklus. Standar: 10.000 ms / 10 detik).
* `intervalKedip = 3000;` (Durasi siklus kedipan mata biasa. Standar: 3.000 ms / 3 detik).

## 🚀 Cara Penggunaan
1. Rangkai komponen sesuai dengan tabel Konfigurasi Pin di atas.
2. Pastikan *library* wajib sudah terinstal di Arduino IDE: `Adafruit_GFX`, `Adafruit_SSD1306`, dan `ESP32Servo`.
3. Buka kode ini menggunakan Arduino IDE.
4. Pilih *board* ESP32 yang sesuai di Arduino IDE.
5. *Compile* dan *Upload* kode ke mikrokontroler ESP32.
6. Taruh robot di lantai, nyalakan sumber daya, dan perhatikan bagaimana robot mengeksplorasi ruangan!
