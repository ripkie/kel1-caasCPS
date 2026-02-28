#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define I2C_SDA 8
#define I2C_SCL 9

#define TRIG_PIN 39
#define ECHO_PIN 38
#define SHOCK_PIN 21

// RGB pins (common cathode)
#define R_PIN 17
#define G_PIN 16
#define B_PIN 15

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Jika shock kamu kebalik (LOW = getar), ubah jadi true
bool ACTIVE_LOW_SHOCK = false;

// =======================
// Fungsi baca jarak
// =======================
float readDistanceCM()
{
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000); // timeout 30ms (~5m)
  if (duration == 0)
    return -1;

  float distance = duration * 0.0343 / 2.0;
  return distance;
}

void setup()
{
  Serial.begin(115200);

  Wire.begin(I2C_SDA, I2C_SCL);
  lcd.init();
  lcd.backlight();

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(SHOCK_PIN, INPUT);

  pinMode(R_PIN, OUTPUT);
  pinMode(G_PIN, OUTPUT);
  pinMode(B_PIN, OUTPUT);

  // Matikan RGB awal
  digitalWrite(R_PIN, LOW);
  digitalWrite(G_PIN, LOW);
  digitalWrite(B_PIN, LOW);

  lcd.setCursor(0, 0);
  lcd.print("BMKG System");
  lcd.setCursor(0, 1);
  lcd.print("Starting...");
  delay(1500);
  lcd.clear();
}

void loop()
{
  float distance = readDistanceCM();

  int shockRaw = digitalRead(SHOCK_PIN);
  bool shockDetected = ACTIVE_LOW_SHOCK ? (shockRaw == LOW) : (shockRaw == HIGH);

  // ===== LED Merah =====
  if (shockDetected)
  {
    digitalWrite(R_PIN, HIGH); // merah nyala
  }
  else
  {
    digitalWrite(R_PIN, LOW); // merah mati
  }
  // (opsional) pastikan G/B mati
  digitalWrite(G_PIN, LOW);
  digitalWrite(B_PIN, LOW);

  // ===== Serial Monitor =====
  Serial.print("Jarak: ");
  Serial.print(distance);
  Serial.print(" cm | Shock: ");
  Serial.println(shockDetected ? "YES" : "NO");

  // ===== LCD =====
  lcd.setCursor(0, 0);
  lcd.print("Jarak: ");
  if (distance < 0)
  {
    lcd.print("Error     ");
  }
  else
  {
    lcd.print(distance, 1);
    lcd.print(" cm   ");
  }

  lcd.setCursor(0, 1);
  if (shockDetected)
  {
    lcd.print("GEMPAA!!!       ");
  }
  else
  {
    lcd.print("Normal          ");
  }

  delay(500);
}