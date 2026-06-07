Proyek ini adalah kode sumber untuk membuat robot pintar berbasis ESP32 yang dapat berjalan dan menghindari rintangan secara otomatis. Robot ini dilengkapi dengan layar OLED SSD1306 sebagai "wajah" yang menampilkan berbagai ekspresi organik dan sangat mulus (ultra-smooth).

Berbeda dengan animasi OLED pada umumnya yang menggunakan delay dan pergantian frame kaku, proyek ini menggunakan perhitungan matematika fisika (gelombang sinus/trigonometri) dan fungsi millis() untuk merender ekspresi secara real-time tanpa membuat sistem robot mengalami lag.

✨ Fitur Utama:

Penghindar Rintangan Cerdas: Menggunakan sensor ultrasonik HC-SR04 yang dipasang di atas motor servo. Saat mendeteksi halangan, robot akan berhenti, menengok ke kiri dan kanan, lalu memilih jalur yang kosong.

Animasi Fisika Real-Time: Menampilkan berbagai siklus ekspresi (Bahagia, Berkedip, Kaget, Malu/Tersipu, Marah, dan Tertawa melompat) dengan transisi gerak yang sangat mulus.

Non-Blocking Sensor: Karena animasi tidak menggunakan fungsi delay(), sensor jarak tetap membaca data dengan kecepatan maksimal, sehingga robot bisa mengerem instan saat ada rintangan mendadak.

Kompensasi Motor: Terdapat variabel pengaturan kecepatan agar robot dapat berjalan lurus sempurna meskipun ada perbedaan kualitas pada kedua motor DC.

🛠️ Komponen yang Dibutuhkan:

Mikrokontroler ESP32

Layar OLED 0.96" (SSD1306) - Komunikasi I2C

Sensor Ultrasonik HC-SR04

Motor Servo (Micro Servo SG90)

Driver Motor DC (L298N atau sejenisnya)

2x Motor DC beserta Roda (Chassis Smart Car 2WD)

Baterai Lipo / 2x 18650 beserta modul step-down (jika diperlukan)

📌 Konfigurasi Pin (Wiring):

Ultrasonik: TRIG (Pin 12), ECHO (Pin 13)

Servo: Pin 14

OLED: SDA (Pin 21 default), SCL (Pin 22 default)

Driver Motor: * ENA (Pin 4), ENB (Pin 19)

Roda Kiri: Maju (Pin 16), Mundur (Pin 17)

Roda Kanan: Maju (Pin 5), Mundur (Pin 18)

📚 Pustaka (Library) yang Wajib Diinstal:
Pastikan kamu sudah menginstal library berikut melalui Arduino Library Manager sebelum melakukan compile:

Adafruit_GFX.h

Adafruit_SSD1306.h

ESP32Servo.h
