#define BLYNK_TEMPLATE_ID "TMPL6UZuKoQ2a"
#define BLYNK_TEMPLATE_NAME "Quickstart Template"
#define BLYNK_AUTH_TOKEN "0Y1A2Sp-nLQyj5ZhSInFMwZiVAPht5WR"

// Aktifkan Serial Monitor untuk debugging
#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

char ssid[] = "Wokwi-GUEST";
char pass[] = "";

const int zone_1_pin = 34;
const int dry = 2700;
const int wet = 0;

BlynkTimer timer;

// Fungsi Kirim Data (Dijalankan setiap 1 detik)
void sendSensorData()
{
  // Baca Sensor
  int raw_value = analogRead(zone_1_pin);

  // Mapping
  int percent_value = map(raw_value, wet, dry, 100, 0);

  // Clamping (Agar tidak minus atau lebih dari 100)
  if (percent_value < 0)
    percent_value = 0;
  if (percent_value > 100)
    percent_value = 100;

  Serial.print("Raw: ");
  Serial.print(raw_value);
  Serial.print(" | Percent: ");
  Serial.print(percent_value);
  Serial.println("%");

  // Kirim ke Blynk
  Blynk.virtualWrite(V0, percent_value);

  // (Opsional) Kirim raw data ke V1
  Blynk.virtualWrite(V1, raw_value);
}

void setup()
{
  Serial.begin(115200);
  pinMode(zone_1_pin, INPUT);

  Serial.println("Menghubungkan ke Blynk...");

  // Fungsi ini otomatis menangani WiFi dan Login Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Set timer kirim data setiap 1 detik (1000L)
  timer.setInterval(1000L, sendSensorData);
}

void loop()
{
  Blynk.run(); // untuk menjaga koneksi
  timer.run(); // untuk menjalankan timer
}
#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>

// ================= WIFI =================
#define WIFI_SSID "iPhone"
#define WIFI_PASSWORD "12341234"

// ================= PIN =================
#define I2C_SDA 8
#define I2C_SCL 9

#define TRIG_PIN 39
#define ECHO_PIN 38
#define SHOCK_PIN 21

#define R_PIN 17
#define G_PIN 16
#define B_PIN 15

LiquidCrystal_I2C lcd(0x27, 16, 2);
bool lcdOK = false;

bool ACTIVE_LOW_SHOCK = false;

unsigned long lastSend = 0;
const unsigned long sendInterval = 2000;

// ================= ULTRASONIC =================
float readDistanceCM()
{
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0)
    return -1;
  return duration * 0.0343f / 2.0f;
}

// ================= WIFI =================
bool connectWiFi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting WiFi");
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 20000)
  {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("\nWiFi Connected!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    return true;
  }
  else
  {
    Serial.println("\nWiFi FAILED (timeout)");
    Serial.print("WiFi status: ");
    Serial.println(WiFi.status());
    return false;
  }
}

void setup()
{
  Serial.begin(115200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(SHOCK_PIN, INPUT);

  pinMode(R_PIN, OUTPUT);
  pinMode(G_PIN, OUTPUT);
  pinMode(B_PIN, OUTPUT);

  digitalWrite(R_PIN, LOW);
  digitalWrite(G_PIN, LOW);
  digitalWrite(B_PIN, LOW);

  Wire.begin(I2C_SDA, I2C_SCL);
  lcd.init();
  lcd.backlight();
  lcdOK = true;

  lcd.setCursor(0, 0);
  lcd.print("BMKG System");
  lcd.setCursor(0, 1);
  lcd.print("Starting...");
  delay(1200);
  lcd.clear();

  connectWiFi();
  // initFirebase();
}

void loop()
{
  float distance = readDistanceCM();

  int shockRaw = digitalRead(SHOCK_PIN);
  bool shockDetected = ACTIVE_LOW_SHOCK ? (shockRaw == LOW) : (shockRaw == HIGH);

  bool ledStatus = shockDetected;

  // LED Merah
  digitalWrite(R_PIN, ledStatus ? HIGH : LOW);
  digitalWrite(G_PIN, LOW);
  digitalWrite(B_PIN, LOW);

  // LCD
  lcd.setCursor(0, 0);
  lcd.print("Jarak: ");
  if (distance < 0)
    lcd.print("Error     ");
  else
  {
    lcd.print(distance, 1);
    lcd.print(" cm   ");
  }

  lcd.setCursor(0, 1);
  lcd.print(shockDetected ? "GEMPAA!!!       " : "Normal          ");

  // Serial
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.print(" | Shock: ");
  Serial.print(shockDetected);
  Serial.print(" | LED: ");
  Serial.println(ledStatus);
}