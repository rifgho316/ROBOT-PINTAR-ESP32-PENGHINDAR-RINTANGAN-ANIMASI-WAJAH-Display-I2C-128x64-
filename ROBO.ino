#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP32Servo.h>

#define LEBAR_LAYAR 128
#define TINGGI_LAYAR 64 
#define RESET_OLED -1
Adafruit_SSD1306 layar(LEBAR_LAYAR, TINGGI_LAYAR, &Wire, RESET_OLED);

const int GESER_Y = 10; 

#define PIN_TRIG 12
#define PIN_ECHO 13
#define PIN_SERVO 14
#define PIN_ENA 4
#define MOTOR_KIRI_MAJU 16  
#define MOTOR_KIRI_MUNDUR 17  
#define MOTOR_KANAN_MAJU 5  
#define MOTOR_KANAN_MUNDUR 18 
#define PIN_ENB 19

Servo servoKepala;

const int WAKTU_MUNDUR_10CM = 400; 
const int WAKTU_BELOK = 500;       
const int KECEPATAN_BELOK = 80;    
const int KECEPATAN_JALAN_KIRI = 69;  
const int KECEPATAN_JALAN_KANAN = 60;  

unsigned long waktuTerakhirKedip = 0;
const long intervalKedip = 3000; 
unsigned long mulaiKedip = 0;
bool sedangKedip = false;

unsigned long waktuTerakhirAnimasi = 0;
const long intervalAnimasi = 10000; 
bool sedangAnimasi = false;
unsigned long mulaiAnimasi = 0;
int jenisAnimasi = 0; 

void setup() {
  Serial.begin(115200);
  
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  pinMode(PIN_ENA, OUTPUT);
  pinMode(PIN_ENB, OUTPUT);
  pinMode(MOTOR_KIRI_MAJU, OUTPUT);
  pinMode(MOTOR_KIRI_MUNDUR, OUTPUT);
  pinMode(MOTOR_KANAN_MAJU, OUTPUT);
  pinMode(MOTOR_KANAN_MUNDUR, OUTPUT);

  servoKepala.attach(PIN_SERVO);
  servoKepala.write(90); 

  if(!layar.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("OLED gagal terhubung!"));
    for(;;);
  }
  
  layar.clearDisplay();
  ekspresiBahagia(); 
  delay(2000); 
}

void loop() {
  float jarakDepan = bacaJarak();

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
  else {
    servoKepala.write(90); 
    maju();
    
    unsigned long waktuSekarang = millis();

    if (waktuSekarang - waktuTerakhirAnimasi >= intervalAnimasi && !sedangAnimasi && !sedangKedip) {
      sedangAnimasi = true;
      mulaiAnimasi = waktuSekarang;
      waktuTerakhirAnimasi = waktuSekarang;
      
      jenisAnimasi++;
      if (jenisAnimasi > 3) jenisAnimasi = 1;
    }

    if (sedangAnimasi) {
      unsigned long durasiAnimasi = waktuSekarang - mulaiAnimasi;
      
      if (durasiAnimasi <= 3000) {
        if (jenisAnimasi == 1) ekspresiNgomongMulus(durasiAnimasi);
        else if (jenisAnimasi == 2) ekspresiMaluMulus(durasiAnimasi);
        else if (jenisAnimasi == 3) ekspresiTertawaMulus(durasiAnimasi);
      } else {
        sedangAnimasi = false; 
        waktuTerakhirKedip = waktuSekarang;
      }
    }
    else {
      if (waktuSekarang - waktuTerakhirKedip >= intervalKedip && !sedangKedip) {
        sedangKedip = true;
        mulaiKedip = waktuSekarang;
        waktuTerakhirKedip = waktuSekarang;
      }

      if (sedangKedip) {
        unsigned long durasiKedip = waktuSekarang - mulaiKedip;
        if (durasiKedip <= 300) { 
          ekspresiBerkedipMulus(durasiKedip);
        } else {
          sedangKedip = false;
        }
      } else {
        ekspresiBahagia(); 
      }
    }
  }
}

float bacaJarak() {
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);
  long durasi = pulseIn(PIN_ECHO, HIGH);
  return durasi * 0.034 / 2;
}

void ekspresiBahagia() {
  layar.clearDisplay();
  layar.fillRoundRect(35, 15 + GESER_Y, 18, 30, 8, SSD1306_WHITE); 
  layar.fillRoundRect(75, 15 + GESER_Y, 18, 30, 8, SSD1306_WHITE); 
  layar.fillCircle(64, 45 + GESER_Y, 5, SSD1306_WHITE); 
  layar.fillCircle(64, 43 + GESER_Y, 5, SSD1306_BLACK); 
  layar.display();
}

void ekspresiBerkedipMulus(unsigned long waktu) {
  layar.clearDisplay();
  float proporsi = (waktu < 150) ? (waktu / 150.0) : ((300.0 - waktu) / 150.0); 
  
  int tinggiLayar = 30 - (proporsi * 26); 
  int posisiY = 15 + GESER_Y + (proporsi * 13); 
  
  layar.fillRoundRect(35, posisiY, 18, tinggiLayar, 2 + ((1.0-proporsi)*6), SSD1306_WHITE);
  layar.fillRoundRect(75, posisiY, 18, tinggiLayar, 2 + ((1.0-proporsi)*6), SSD1306_WHITE);
  
  layar.fillCircle(64, 45 + GESER_Y, 5, SSD1306_WHITE); 
  layar.fillCircle(64, 43 + GESER_Y, 5, SSD1306_BLACK); 
  layar.display();
}

void ekspresiNgomongMulus(unsigned long waktu) {
  layar.clearDisplay();
  
  float sudut = (waktu / 3000.0) * PI * 4.0; 
  float gelombang = (sin(sudut - (PI/2)) + 1.0) / 2.0; 
  
  int lebarMata = 18 + (gelombang * 6); 
  int tinggiMata = 30 + (gelombang * 4); 
  int posisiXMata = 35 - (gelombang * 3); 
  int posisiYMata = 15 + GESER_Y - (gelombang * 2);
  
  layar.fillRoundRect(posisiXMata, posisiYMata, lebarMata, tinggiMata, 8, SSD1306_WHITE); 
  layar.fillRoundRect(128 - posisiXMata - lebarMata, posisiYMata, lebarMata, tinggiMata, 8, SSD1306_WHITE); 
  
  int lebarMulut = 4 + (gelombang * 12); 
  int tinggiMulut = 4 + (gelombang * 14);
  layar.fillRoundRect(64 - lebarMulut/2, 45 + GESER_Y - tinggiMulut/2, lebarMulut, tinggiMulut, 3 + (gelombang*2), SSD1306_WHITE);
  
  layar.display();
}

void ekspresiMaluMulus(unsigned long waktu) {
  layar.clearDisplay();
  
  float proporsi = (waktu < 500) ? (waktu / 500.0) : ((waktu < 2500) ? 1.0 : ((3000.0 - waktu) / 500.0));
  
  float ayunan = (waktu > 500 && waktu < 2500) ? (sin(((waktu - 500) / 2000.0) * PI) * 4.0) : 0;
  int posisiY = GESER_Y + ayunan;
  
  layar.fillRoundRect(35, 15 + posisiY, 18, 30, 8, SSD1306_WHITE); 
  layar.fillRoundRect(75, 15 + posisiY, 18, 30, 8, SSD1306_WHITE);
  
  int tinggiTutup = proporsi * 15; 
  layar.fillRect(30, 45 + posisiY - tinggiTutup, 28, tinggiTutup + 2, SSD1306_BLACK);
  layar.fillRect(70, 45 + posisiY - tinggiTutup, 28, tinggiTutup + 2, SSD1306_BLACK);
  
  int lebarMulut = 10 + (proporsi * 4); 
  int tinggiMulut = 4 + (proporsi * 4);  
  layar.fillRoundRect(64 - lebarMulut/2, 45 + posisiY - tinggiMulut/2, lebarMulut, tinggiMulut, 3, SSD1306_WHITE);
  layar.fillRect(64 - lebarMulut/2, 45 + posisiY - tinggiMulut/2 - 2, lebarMulut, tinggiMulut/2 + 2, SSD1306_BLACK); 
  
  if (proporsi > 0.5) {
    int posisiYGaris = 30 + posisiY + (1.0 - proporsi) * 10; 
    for(int i=0; i<3; i++) {
      layar.drawLine(24+(i*4), posisiYGaris, 20+(i*4), posisiYGaris + 6, SSD1306_WHITE);
      layar.drawLine(96+(i*4), posisiYGaris, 100+(i*4), posisiYGaris + 6, SSD1306_WHITE);
    }
  }
  layar.display();
}

void ekspresiTertawaMulus(unsigned long waktu) {
  layar.clearDisplay();
  
  float proporsi = (waktu < 500) ? (waktu / 500.0) : ((waktu < 2500) ? 1.0 : ((3000.0 - waktu) / 500.0));
  
  float ayunan = (waktu > 500 && waktu < 2500) ? (-abs(sin(((waktu - 500) / 2000.0) * 2.0 * PI)) * 12.0) : 0;
  int posisiY = GESER_Y + ayunan;
  
  if (proporsi < 0.8) {
    int lebarMata = 18 - (proporsi * 14); 
    layar.fillRoundRect(44 - lebarMata/2, 15 + posisiY, lebarMata, 30, lebarMata/2, SSD1306_WHITE);
    layar.fillRoundRect(84 - lebarMata/2, 15 + posisiY, lebarMata, 30, lebarMata/2, SSD1306_WHITE);
  } else {
    layar.fillTriangle(25, 24 + posisiY, 50, 14 + posisiY, 50, 19 + posisiY, SSD1306_WHITE);
    layar.fillTriangle(103, 24 + posisiY, 78, 14 + posisiY, 78, 19 + posisiY, SSD1306_WHITE);
  }
  
  int lebarMulut = 10 + (proporsi * 34); 
  int tinggiMulut = 4 + (proporsi * 21);  
  layar.fillRoundRect(64 - lebarMulut/2, 42 + posisiY - tinggiMulut/2 + (proporsi*3), lebarMulut, tinggiMulut, 2+(proporsi*8), SSD1306_WHITE);
  layar.fillRect(64 - lebarMulut/2, 42 + posisiY - tinggiMulut/2 - 2 + (proporsi*3), lebarMulut, tinggiMulut/2 + 2, SSD1306_BLACK); 
  
  if (proporsi > 0.8) {
    layar.fillRoundRect(46, 35 + posisiY, 36, 16, 6, SSD1306_BLACK);  
    layar.fillRect(42, 35 + posisiY, 44, 3, SSD1306_WHITE); 
  }
  
  layar.display();
}

void ekspresiMelihatKanan() {
  layar.clearDisplay();
  layar.fillRoundRect(10, 15 + GESER_Y, 18, 30, 8, SSD1306_WHITE);
  layar.fillRoundRect(50, 15 + GESER_Y, 18, 30, 8, SSD1306_WHITE); 
  layar.fillTriangle(5, 10 + GESER_Y, 35, 10 + GESER_Y, 35, 25 + GESER_Y, SSD1306_BLACK); 
  layar.fillTriangle(73, 10 + GESER_Y, 43, 10 + GESER_Y, 43, 25 + GESER_Y, SSD1306_BLACK); 
  layar.fillCircle(33, 45 + GESER_Y, 5, SSD1306_WHITE); 
  layar.fillCircle(33, 47 + GESER_Y, 5, SSD1306_BLACK); 
  layar.display();
}

void ekspresiMelihatKiri() {
  layar.clearDisplay();
  layar.fillRoundRect(60, 15 + GESER_Y, 18, 30, 8, SSD1306_WHITE); 
  layar.fillRoundRect(100, 15 + GESER_Y, 18, 30, 8, SSD1306_WHITE); 
  layar.fillTriangle(55, 10 + GESER_Y, 85, 10 + GESER_Y, 85, 25 + GESER_Y, SSD1306_BLACK); 
  layar.fillTriangle(123, 10 + GESER_Y, 93, 10 + GESER_Y, 93, 25 + GESER_Y, SSD1306_BLACK); 
  layar.fillCircle(93, 45 + GESER_Y, 5, SSD1306_WHITE); 
  layar.fillCircle(93, 47 + GESER_Y, 5, SSD1306_BLACK); 
  layar.display();
}

void ekspresiMarah() {
  layar.clearDisplay();
  layar.fillRoundRect(35, 15 + GESER_Y, 18, 30, 8, SSD1306_WHITE); 
  layar.fillRoundRect(75, 15 + GESER_Y, 18, 30, 8, SSD1306_WHITE); 
  layar.fillTriangle(30, 10 + GESER_Y, 60, 10 + GESER_Y, 60, 25 + GESER_Y, SSD1306_BLACK); 
  layar.fillTriangle(98, 10 + GESER_Y, 68, 10 + GESER_Y, 68, 25 + GESER_Y, SSD1306_BLACK); 
  layar.fillCircle(64, 45 + GESER_Y, 5, SSD1306_WHITE); 
  layar.fillCircle(64, 47 + GESER_Y, 5, SSD1306_BLACK); 
  layar.display();
}

void ekspresiDatar() {
  layar.clearDisplay();
  layar.fillRoundRect(35, 15 + GESER_Y, 18, 30, 8, SSD1306_WHITE); 
  layar.fillRoundRect(75, 15 + GESER_Y, 18, 30, 8, SSD1306_WHITE); 
  layar.fillTriangle(20, 5 + GESER_Y, 60, 5 + GESER_Y, 20, 25 + GESER_Y, SSD1306_BLACK); 
  layar.fillTriangle(108, 5 + GESER_Y, 68, 5 + GESER_Y, 108, 25 + GESER_Y, SSD1306_BLACK); 
  layar.fillCircle(64, 45 + GESER_Y, 5, SSD1306_WHITE); 
  layar.fillCircle(64, 47 + GESER_Y, 5, SSD1306_BLACK); 
  layar.display();
}

void aturKecepatan(int kecepatanKiri, int kecepatanKanan) {
  analogWrite(PIN_ENA, kecepatanKiri);
  analogWrite(PIN_ENB, kecepatanKanan);
}

void maju() {
  aturKecepatan(KECEPATAN_JALAN_KIRI, KECEPATAN_JALAN_KANAN);
  digitalWrite(MOTOR_KIRI_MAJU, HIGH); digitalWrite(MOTOR_KIRI_MUNDUR, LOW);
  digitalWrite(MOTOR_KANAN_MAJU, HIGH); digitalWrite(MOTOR_KANAN_MUNDUR, LOW);
}

void mundur() {
  aturKecepatan(KECEPATAN_JALAN_KIRI, KECEPATAN_JALAN_KANAN);
  digitalWrite(MOTOR_KIRI_MAJU, LOW);  digitalWrite(MOTOR_KIRI_MUNDUR, HIGH);
  digitalWrite(MOTOR_KANAN_MAJU, LOW); digitalWrite(MOTOR_KANAN_MUNDUR, HIGH);
}

void belokKiri() {
  aturKecepatan(KECEPATAN_BELOK, KECEPATAN_BELOK);
  digitalWrite(MOTOR_KIRI_MAJU, LOW);  digitalWrite(MOTOR_KIRI_MUNDUR, HIGH);
  digitalWrite(MOTOR_KANAN_MAJU, HIGH); digitalWrite(MOTOR_KANAN_MUNDUR, LOW);
}

void belokKanan() {
  aturKecepatan(KECEPATAN_BELOK, KECEPATAN_BELOK);
  digitalWrite(MOTOR_KIRI_MAJU, HIGH); digitalWrite(MOTOR_KIRI_MUNDUR, LOW);
  digitalWrite(MOTOR_KANAN_MAJU, LOW);  digitalWrite(MOTOR_KANAN_MUNDUR, HIGH);
}

void berhenti() {
  aturKecepatan(0, 0); 
  digitalWrite(MOTOR_KIRI_MAJU, LOW); digitalWrite(MOTOR_KIRI_MUNDUR, LOW);
  digitalWrite(MOTOR_KANAN_MAJU, LOW); digitalWrite(MOTOR_KANAN_MUNDUR, LOW);
}
