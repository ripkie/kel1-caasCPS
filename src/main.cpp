#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ====== UBAH SESUAI PUNYA KAMU ======
#define I2C_SDA 8
#define I2C_SCL 9

const int SHOCK_PIN = 21;

// Alamat LCD paling umum 0x27, kadang 0x3F
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Kalau shock kamu kebalik (LOW = getar), ubah ke true
bool ACTIVE_LOW_SHOCK = false;
// ===================================

void setup()
{
  Serial.begin(9600);

  // I2C custom pin
  Wire.begin(I2C_SDA, I2C_SCL);

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Shock Sensor");
  lcd.setCursor(0, 1);
  lcd.print("Init...");
  delay(1000);

  // Kalau modul shock kamu “pasif” dan butuh pull-up, ganti ke INPUT_PULLUP
  pinMode(SHOCK_PIN, INPUT);

  lcd.clear();
}

void loop()
{
  int raw = digitalRead(SHOCK_PIN);

  bool shockDetected;
  if (ACTIVE_LOW_SHOCK)
  {
    shockDetected = (raw == LOW);
  }
  else
  {
    shockDetected = (raw == HIGH);
  }

  // Serial
  Serial.print("Shock raw: ");
  Serial.print(raw);
  Serial.print(" | Detected: ");
  Serial.println(shockDetected ? "YES" : "NO");

  // LCD
  lcd.setCursor(0, 0);
  lcd.print("Raw: ");
  lcd.print(raw);
  lcd.print("           "); // clear sisa char

  lcd.setCursor(0, 1);
  if (shockDetected)
  {
    lcd.print("GETARAN TERDET! ");
  }
  else
  {
    lcd.print("Normal          ");
  }

  delay(200);
}