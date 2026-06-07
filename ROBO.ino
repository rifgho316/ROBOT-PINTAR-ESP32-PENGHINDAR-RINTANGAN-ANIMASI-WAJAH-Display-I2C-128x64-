#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP32Servo.h>

// ================= PENGATURAN OLED =================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64 
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Offset sumbu Y agar animasi dasar berada di tengah layar
const int Y_OFFSET = 10; 

// ================= DEFINISI PIN (ESP32) =================
#define TRIG_PIN 12
#define ECHO_PIN 13
#define SERVO_PIN 14
#define ENA_PIN 4
#define MOTOR_KIRI_F 16  
#define MOTOR_KIRI_B 17  
#define MOTOR_KANAN_F 5  
#define MOTOR_KANAN_B 18 
#define ENB_PIN 19

// ================= OBJEK & VARIABEL =================
Servo servoKepala;

// Kalibrasi Kecepatan & Waktu
const int WAKTU_MUNDUR_10CM = 400; 
const int WAKTU_BELOK = 500;       
const int KECEPATAN_BELOK = 80;    
const int KECEPATAN_JALAN_KIRI = 69;  
const int KECEPATAN_JALAN_KANAN = 60;  

// Variabel Animasi Kedip (Idle)
unsigned long waktuTerakhirKedip = 0;
const long intervalKedip = 3000; 
unsigned long mulaiKedip = 0;
bool sedangKedip = false;

// Variabel Sistem Siklus Animasi (Setiap 10 Detik)
unsigned long waktuTerakhirAnimasi = 0;
const long intervalAnimasi = 10000; 
bool sedangAnimasi = false;
unsigned long mulaiAnimasi = 0;
int jenisAnimasi = 0; // 1 = Kaget, 2 = Malu, 3 = Tertawa

// ================= SETUP AWAL =================
void setup() {
  Serial.begin(115200);
  
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(ENA_PIN, OUTPUT);
  pinMode(ENB_PIN, OUTPUT);
  pinMode(MOTOR_KIRI_F, OUTPUT);
  pinMode(MOTOR_KIRI_B, OUTPUT);
  pinMode(MOTOR_KANAN_F, OUTPUT);
  pinMode(MOTOR_KANAN_B, OUTPUT);

  servoKepala.attach(SERVO_PIN);
  servoKepala.write(90); 

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("OLED gagal terhubung!"));
    for(;;);
  }
  
  display.clearDisplay();
  ekspresiBahagia(); 
  delay(2000); 
}

// ================= LOOP UTAMA =================
void loop() {
  float jarakDepan = bacaJarak();

  // --- LOGIKA JIKA TERHALANG OBJEK ---
  if (jarakDepan > 0 && jarakDepan <= 20) {
    sedangAnimasi = false; 
    sedangKedip = false;
    
    berhenti();
    ekspresiMarah(); 
    delay(300); 

    mundur();
    delay(WAKTU_MUNDUR_10CM);
    berhenti();
    delay(100); 

    servoKepala.write(0); 
    ekspresiMelihatKanan(); 
    delay(400); 
    float jarakKanan = bacaJarak();

    servoKepala.write(180);
    ekspresiMelihatKiri(); 
    delay(600); 
    float jarakKiri = bacaJarak();

    servoKepala.write(90);
    ekspresiDatar(); 
    delay(400); 

    if (jarakKiri >= 30 && jarakKanan < 30) {
      belokKiri(); delay(WAKTU_BELOK);
    } 
    else if (jarakKanan >= 30 && jarakKiri < 30) {
      belokKanan(); delay(WAKTU_BELOK);
    } 
    else {
      belokKanan(); delay(WAKTU_BELOK);
    }
    berhenti(); 
    delay(200);
  } 
  
  // --- LOGIKA JALAN LURUS (IDLE & ANIMASI ULTRA SMOOTH) ---
  else {
    servoKepala.write(90); 
    maju();
    
    unsigned long waktuSekarang = millis();

    // 1. Cek Trigger Animasi 10 Detik
    if (waktuSekarang - waktuTerakhirAnimasi >= intervalAnimasi && !sedangAnimasi && !sedangKedip) {
      sedangAnimasi = true;
      mulaiAnimasi = waktuSekarang;
      waktuTerakhirAnimasi = waktuSekarang;
      
      jenisAnimasi++;
      if (jenisAnimasi > 3) jenisAnimasi = 1;
    }

    // 2. Jalankan Animasi Smooth Berdasarkan Rumus Waktu
    if (sedangAnimasi) {
      unsigned long durasiAnimasi = waktuSekarang - mulaiAnimasi;
      
      if (durasiAnimasi <= 3000) {
        if (jenisAnimasi == 1) ekspresiNgomongSmooth(durasiAnimasi);
        else if (jenisAnimasi == 2) ekspresiMaluSmooth(durasiAnimasi);
        else if (jenisAnimasi == 3) ekspresiTertawaSmooth(durasiAnimasi);
      } else {
        sedangAnimasi = false; 
        waktuTerakhirKedip = waktuSekarang; // Reset waktu kedip
      }
    }
    
    // 3. Logika Kedip Normal (Smooth Blink)
    else {
      if (waktuSekarang - waktuTerakhirKedip >= intervalKedip && !sedangKedip) {
        sedangKedip = true;
        mulaiKedip = waktuSekarang;
        waktuTerakhirKedip = waktuSekarang;
      }

      if (sedangKedip) {
        unsigned long durasiKedip = waktuSekarang - mulaiKedip;
        if (durasiKedip <= 300) { // Berkedip selama 300ms
          ekspresiBlinkContinuous(durasiKedip);
        } else {
          sedangKedip = false;
        }
      } else {
        ekspresiBahagia(); // Standby
      }
    }
  }
}

// ================= FUNGSI BACA SENSOR =================
float bacaJarak() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long durasi = pulseIn(ECHO_PIN, HIGH);
  return durasi * 0.034 / 2;
}

// ================= FUNGSI ANIMASI OLED =================

void ekspresiBahagia() {
  display.clearDisplay();
  display.fillRoundRect(35, 15 + Y_OFFSET, 18, 30, 8, SSD1306_WHITE); 
  display.fillRoundRect(75, 15 + Y_OFFSET, 18, 30, 8, SSD1306_WHITE); 
  display.fillCircle(64, 45 + Y_OFFSET, 5, SSD1306_WHITE); 
  display.fillCircle(64, 43 + Y_OFFSET, 5, SSD1306_BLACK); 
  display.display();
}

// === FUNGSI MATEMATIKA ANIMASI MULUS (TANPA DELAY SAMA SEKALI) ===

// 1. Berkedip Real-Time (Smooth Close & Open)
void ekspresiBlinkContinuous(unsigned long t) {
  display.clearDisplay();
  float p = (t < 150) ? (t / 150.0) : ((300.0 - t) / 150.0); 
  
  int h = 30 - (p * 26); // Tinggi mata mengecil sampai 4px
  int y = 15 + Y_OFFSET + (p * 13); // Mata turun ke tengah
  
  display.fillRoundRect(35, y, 18, h, 2 + ((1.0-p)*6), SSD1306_WHITE);
  display.fillRoundRect(75, y, 18, h, 2 + ((1.0-p)*6), SSD1306_WHITE);
  
  display.fillCircle(64, 45 + Y_OFFSET, 5, SSD1306_WHITE); 
  display.fillCircle(64, 43 + Y_OFFSET, 5, SSD1306_BLACK); 
  display.display();
}

// 2. Animasi Kaget/Ngomong (Menggunakan Gelombang Sinus)
void ekspresiNgomongSmooth(unsigned long t) {
  display.clearDisplay();
  
  // Gelombang Buka-Tutup sebanyak 2 siklus dalam 3 detik
  float sudut = (t / 3000.0) * PI * 4.0; 
  float wave = (sin(sudut - (PI/2)) + 1.0) / 2.0; // Hasil 0.0 sampai 1.0 sangat mulus
  
  int ew = 18 + (wave * 6); // Mata membesar (18 ke 24)
  int eh = 30 + (wave * 4); // Tinggi membesar (30 ke 34)
  int ex = 35 - (wave * 3); 
  int ey = 15 + Y_OFFSET - (wave * 2);
  
  display.fillRoundRect(ex, ey, ew, eh, 8, SSD1306_WHITE); // Kiri
  display.fillRoundRect(128 - ex - ew, ey, ew, eh, 8, SSD1306_WHITE); // Kanan
  
  int mw = 4 + (wave * 12); // Mulut O membesar mulus
  int mh = 4 + (wave * 14);
  display.fillRoundRect(64 - mw/2, 45 + Y_OFFSET - mh/2, mw, mh, 3 + (wave*2), SSD1306_WHITE);
  
  display.display();
}

// 3. Animasi Malu (Interpolasi Transisi & Mengayun/Bobbing)
void ekspresiMaluSmooth(unsigned long t) {
  display.clearDisplay();
  
  // Transisi masuk (0-500ms), Tahan (500-2500ms), Transisi Keluar (2500-3000ms)
  float p = (t < 500) ? (t / 500.0) : ((t < 2500) ? 1.0 : ((3000.0 - t) / 500.0));
  
  // Gerakan tersipu turun perlahan (Hanya aktif di tengah)
  float bob = (t > 500 && t < 2500) ? (sin(((t - 500) / 2000.0) * PI) * 4.0) : 0;
  int y = Y_OFFSET + bob;
  
  display.fillRoundRect(35, 15 + y, 18, 30, 8, SSD1306_WHITE); 
  display.fillRoundRect(75, 15 + y, 18, 30, 8, SSD1306_WHITE);
  
  // Penutup mata hitam bergerak mulus memotong dari bawah menjadi ^ ^
  int tutupH = p * 15; 
  display.fillRect(30, 45 + y - tutupH, 28, tutupH + 2, SSD1306_BLACK);
  display.fillRect(70, 45 + y - tutupH, 28, tutupH + 2, SSD1306_BLACK);
  
  // Transisi mulut senyum lebar
  int mw = 10 + (p * 4); 
  int mh = 4 + (p * 4);  
  display.fillRoundRect(64 - mw/2, 45 + y - mh/2, mw, mh, 3, SSD1306_WHITE);
  display.fillRect(64 - mw/2, 45 + y - mh/2 - 2, mw, mh/2 + 2, SSD1306_BLACK); 
  
  // Pipi (Slide-in mulus dari bawah)
  if (p > 0.5) {
    int garisY = 30 + y + (1.0 - p) * 10; 
    for(int i=0; i<3; i++) {
      display.drawLine(24+(i*4), garisY, 20+(i*4), garisY + 6, SSD1306_WHITE);
      display.drawLine(96+(i*4), garisY, 100+(i*4), garisY + 6, SSD1306_WHITE);
    }
  }
  display.display();
}

// 4. Animasi Tertawa (Interpolasi & Gelombang Lompat Fisika)
void ekspresiTertawaSmooth(unsigned long t) {
  display.clearDisplay();
  
  float p = (t < 500) ? (t / 500.0) : ((t < 2500) ? 1.0 : ((3000.0 - t) / 500.0));
  
  // Efek lompat 2 kali menggunakan Absolute Sine Wave Fisika
  float bob = (t > 500 && t < 2500) ? (-abs(sin(((t - 500) / 2000.0) * 2.0 * PI)) * 12.0) : 0;
  int y = Y_OFFSET + bob;
  
  // Mata morph menyipit
  if (p < 0.8) {
    int ew = 18 - (p * 14); // Lebar mata mengecil
    display.fillRoundRect(44 - ew/2, 15 + y, ew, 30, ew/2, SSD1306_WHITE);
    display.fillRoundRect(84 - ew/2, 15 + y, ew, 30, ew/2, SSD1306_WHITE);
  } else {
    display.fillTriangle(25, 24 + y, 50, 14 + y, 50, 19 + y, SSD1306_WHITE);
    display.fillTriangle(103, 24 + y, 78, 14 + y, 78, 19 + y, SSD1306_WHITE);
  }
  
  // Mulut Morph Mangap
  int mw = 10 + (p * 34); 
  int mh = 4 + (p * 21);  
  display.fillRoundRect(64 - mw/2, 42 + y - mh/2 + (p*3), mw, mh, 2+(p*8), SSD1306_WHITE);
  display.fillRect(64 - mw/2, 42 + y - mh/2 - 2 + (p*3), mw, mh/2 + 2, SSD1306_BLACK); 
  
  if (p > 0.8) {
    display.fillRoundRect(46, 35 + y, 36, 16, 6, SSD1306_BLACK);  // Rongga
    display.fillRect(42, 35 + y, 44, 3, SSD1306_WHITE); // Gigi
  }
  
  display.display();
}

// === FUNGSI EKSPRESI CEK JALAN (Kiri, Kanan, Marah) ===

void ekspresiMelihatKanan() {
  display.clearDisplay();
  display.fillRoundRect(10, 15 + Y_OFFSET, 18, 30, 8, SSD1306_WHITE);
  display.fillRoundRect(50, 15 + Y_OFFSET, 18, 30, 8, SSD1306_WHITE); 
  display.fillTriangle(5, 10 + Y_OFFSET, 35, 10 + Y_OFFSET, 35, 25 + Y_OFFSET, SSD1306_BLACK); 
  display.fillTriangle(73, 10 + Y_OFFSET, 43, 10 + Y_OFFSET, 43, 25 + Y_OFFSET, SSD1306_BLACK); 
  display.fillCircle(33, 45 + Y_OFFSET, 5, SSD1306_WHITE); 
  display.fillCircle(33, 47 + Y_OFFSET, 5, SSD1306_BLACK); 
  display.display();
}

void ekspresiMelihatKiri() {
  display.clearDisplay();
  display.fillRoundRect(60, 15 + Y_OFFSET, 18, 30, 8, SSD1306_WHITE); 
  display.fillRoundRect(100, 15 + Y_OFFSET, 18, 30, 8, SSD1306_WHITE); 
  display.fillTriangle(55, 10 + Y_OFFSET, 85, 10 + Y_OFFSET, 85, 25 + Y_OFFSET, SSD1306_BLACK); 
  display.fillTriangle(123, 10 + Y_OFFSET, 93, 10 + Y_OFFSET, 93, 25 + Y_OFFSET, SSD1306_BLACK); 
  display.fillCircle(93, 45 + Y_OFFSET, 5, SSD1306_WHITE); 
  display.fillCircle(93, 47 + Y_OFFSET, 5, SSD1306_BLACK); 
  display.display();
}

void ekspresiMarah() {
  display.clearDisplay();
  display.fillRoundRect(35, 15 + Y_OFFSET, 18, 30, 8, SSD1306_WHITE); 
  display.fillRoundRect(75, 15 + Y_OFFSET, 18, 30, 8, SSD1306_WHITE); 
  display.fillTriangle(30, 10 + Y_OFFSET, 60, 10 + Y_OFFSET, 60, 25 + Y_OFFSET, SSD1306_BLACK); 
  display.fillTriangle(98, 10 + Y_OFFSET, 68, 10 + Y_OFFSET, 68, 25 + Y_OFFSET, SSD1306_BLACK); 
  display.fillCircle(64, 45 + Y_OFFSET, 5, SSD1306_WHITE); 
  display.fillCircle(64, 47 + Y_OFFSET, 5, SSD1306_BLACK); 
  display.display();
}

void ekspresiDatar() {
  display.clearDisplay();
  display.fillRoundRect(35, 15 + Y_OFFSET, 18, 30, 8, SSD1306_WHITE); 
  display.fillRoundRect(75, 15 + Y_OFFSET, 18, 30, 8, SSD1306_WHITE); 
  display.fillTriangle(20, 5 + Y_OFFSET, 60, 5 + Y_OFFSET, 20, 25 + Y_OFFSET, SSD1306_BLACK); 
  display.fillTriangle(108, 5 + Y_OFFSET, 68, 5 + Y_OFFSET, 108, 25 + Y_OFFSET, SSD1306_BLACK); 
  display.fillCircle(64, 45 + Y_OFFSET, 5, SSD1306_WHITE); 
  display.fillCircle(64, 47 + Y_OFFSET, 5, SSD1306_BLACK); 
  display.display();
}

// ================= FUNGSI MOTOR =================
void aturKecepatan(int speedL, int speedR) {
  analogWrite(ENA_PIN, speedL);
  analogWrite(ENB_PIN, speedR);
}

void maju() {
  aturKecepatan(KECEPATAN_JALAN_KIRI, KECEPATAN_JALAN_KANAN);
  digitalWrite(MOTOR_KIRI_F, HIGH); digitalWrite(MOTOR_KIRI_B, LOW);
  digitalWrite(MOTOR_KANAN_F, HIGH); digitalWrite(MOTOR_KANAN_B, LOW);
}

void mundur() {
  aturKecepatan(KECEPATAN_JALAN_KIRI, KECEPATAN_JALAN_KANAN);
  digitalWrite(MOTOR_KIRI_F, LOW);  digitalWrite(MOTOR_KIRI_B, HIGH);
  digitalWrite(MOTOR_KANAN_F, LOW); digitalWrite(MOTOR_KANAN_B, HIGH);
}

void belokKiri() {
  aturKecepatan(KECEPATAN_BELOK, KECEPATAN_BELOK);
  digitalWrite(MOTOR_KIRI_F, LOW);  digitalWrite(MOTOR_KIRI_B, HIGH);
  digitalWrite(MOTOR_KANAN_F, HIGH); digitalWrite(MOTOR_KANAN_B, LOW);
}

void belokKanan() {
  aturKecepatan(KECEPATAN_BELOK, KECEPATAN_BELOK);
  digitalWrite(MOTOR_KIRI_F, HIGH); digitalWrite(MOTOR_KIRI_B, LOW);
  digitalWrite(MOTOR_KANAN_F, LOW);  digitalWrite(MOTOR_KANAN_B, HIGH);
}

void berhenti() {
  aturKecepatan(0, 0); 
  digitalWrite(MOTOR_KIRI_F, LOW); digitalWrite(MOTOR_KIRI_B, LOW);
  digitalWrite(MOTOR_KANAN_F, LOW); digitalWrite(MOTOR_KANAN_B, LOW);
}