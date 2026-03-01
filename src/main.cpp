#define BLYNK_TEMPLATE_ID "TMPL6UZuKoQ2a"
#define BLYNK_TEMPLATE_NAME "Quickstart Template"
#define BLYNK_AUTH_TOKEN "0Y1A2Sp-nLQyj5ZhSInFMwZiVAPht5WR"
#define BLYNK_PRINT Serial

#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

// ===== WiFi =====
char ssid[] = "iPhone";
char pass[] = "12341234";

// ===== Firebase Config =====
#define FIREBASE_API_KEY "AIzaSyAhtWZAOYF3f_r4TOQNdIE5hnBgjKwDCvk"
#define FIREBASE_DATABASE_URL "https://kel1-caascps-acea3-default-rtdb.firebaseio.com"
#define FIREBASE_USER_EMAIL "rifki.dupon07@gmail.com"
#define FIREBASE_USER_PASSWORD "Rifkiwidya6*"

// init pin
#define I2C_SDA 8
#define I2C_SCL 9
#define TRIG_PIN 41
#define ECHO_PIN 38
#define SHOCK_PIN 21
#define R_PIN 17
#define G_PIN 16
#define B_PIN 15

LiquidCrystal_I2C lcd(0x27, 16, 2);

bool ACTIVE_LOW_SHOCK = false;
BlynkTimer timer;

// ===== Firebase objects =====
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// pengaturan lcd
void lcdPrintLine(byte row, const String &msg)
{
  lcd.setCursor(0, row);
  String s = msg;
  while (s.length() < 16)
    s += " ";
  lcd.print(s.substring(0, 16));
}

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

void updateSystem()
{
  float distance = readDistanceCM();
  int shockRaw = digitalRead(SHOCK_PIN);
  bool shockDetected = ACTIVE_LOW_SHOCK ? (shockRaw == LOW) : (shockRaw == HIGH);

  // LED Nyala saat SHOCk
  digitalWrite(R_PIN, shockDetected ? HIGH : LOW);
  digitalWrite(G_PIN, LOW);
  digitalWrite(B_PIN, LOW);

  // Status koneksi
  bool wifiOK = (WiFi.status() == WL_CONNECTED);
  bool blynkOK = Blynk.connected();
  bool firebaseOK = Firebase.ready();

  // LCD
  String line0 = "Jarak:";
  if (distance < 0)
    line0 += "Err";
  else
  {
    line0 += String(distance, 1);
    line0 += " cm";
  }

  String line1 = "Status: ";
  line1 += shockDetected ? "GEMPAA!!!" : "Normal";

  lcdPrintLine(0, line0);
  lcdPrintLine(1, line1);

  // ===== Kirim ke Blynk =====
  if (blynkOK)
  {
    Blynk.virtualWrite(V0, shockDetected ? 1 : 0);
    Blynk.virtualWrite(V1, distance);
    Serial.print("[BLYNK SEND OK] ");
  }
  else
  {
    Serial.print("[BLYNK SKIP] ");
  }

  // send data to firebase
  if (firebaseOK)
  {
    bool ok1 = Firebase.RTDB.setInt(&fbdo, "/devices/esp32s3_01/shock", shockDetected ? 1 : 0);
    bool ok2 = Firebase.RTDB.setFloat(&fbdo, "/devices/esp32s3_01/distance_cm", distance);
    bool ok3 = Firebase.RTDB.setInt(&fbdo, "/devices/esp32s3_01/ts_ms", (int)millis());

    if (ok1 && ok2 && ok3)
    {
      Serial.print("[FIREBASE SEND OK] ");
    }
    else
    {
      Serial.print("[FIREBASE FAIL] ");
      Serial.print(fbdo.errorReason());
      Serial.print(" ");
    }
  }
  else
  {
    Serial.print("[FIREBASE NOT READY] ");
  }

  // Serial Monitor
  Serial.print("Shock=");
  Serial.print(shockDetected ? 1 : 0);
  Serial.print(" Jarak=");
  Serial.print(distance);
  Serial.print("cm WiFi=");
  Serial.print(wifiOK ? "OK" : "NO");
  Serial.print(" Blynk=");
  Serial.print(blynkOK ? "OK" : "NO");
  Serial.print(" FB=");
  Serial.println(firebaseOK ? "OK" : "NO");
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
  digitalWrite(TRIG_PIN, LOW);

  Wire.begin(I2C_SDA, I2C_SCL);
  lcd.init();
  lcd.backlight();

  // LCD: sensor nyala
  lcdPrintLine(0, "BMKG System");
  lcdPrintLine(1, "Sensor nyala");
  Serial.println("[SYS] Sensor nyala");
  delay(800);

  // WiFi connect
  lcdPrintLine(0, "WiFi");
  lcdPrintLine(1, "Connecting...");
  Serial.print("[WiFi] Connecting");

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);

  int retry = 0;
  while (WiFi.status() != WL_CONNECTED && retry < 30)
  {
    delay(500);
    Serial.print(".");
    retry++;
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.print("[WiFi] OK IP: ");
    Serial.println(WiFi.localIP());
    lcdPrintLine(0, "WiFi OK!");
    lcdPrintLine(1, WiFi.localIP().toString());
    delay(1000);
  }
  else
  {
    Serial.println("[WiFi] GAGAL!");
    lcdPrintLine(0, "WiFi GAGAL!");
    lcdPrintLine(1, "Cek SSID/PASS");
    delay(1200);
  }

  // Blynk connect
  lcdPrintLine(0, "Blynk");
  lcdPrintLine(1, "Connecting...");
  Serial.println("[Blynk] Connecting...");
  Blynk.config(BLYNK_AUTH_TOKEN, "blynk.cloud", 80);
  Blynk.connect(5000);

  if (Blynk.connected())
  {
    Serial.println("[Blynk] OK Connected!");
    lcdPrintLine(0, "Blynk OK!");
    lcdPrintLine(1, "Firebase next..");
  }
  else
  {
    Serial.println("[Blynk] GAGAL!");
    lcdPrintLine(0, "Blynk GAGAL!");
    lcdPrintLine(1, "Cek token");
  }
  delay(1000);

  // Firebase init
  lcdPrintLine(0, "Firebase");
  lcdPrintLine(1, "Connecting...");
  Serial.println("[Firebase] Connecting...");

  config.api_key = FIREBASE_API_KEY;
  config.database_url = FIREBASE_DATABASE_URL;

  auth.user.email = FIREBASE_USER_EMAIL;
  auth.user.password = FIREBASE_USER_PASSWORD;

  config.token_status_callback = tokenStatusCallback; // dari addons/TokenHelper.h

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  // tunggu sebentar biar token kebentuk
  unsigned long t0 = millis();
  while (!Firebase.ready() && millis() - t0 < 5000)
  {
    delay(200);
    Serial.print(".");
  }
  Serial.println();

  if (Firebase.ready())
  {
    Serial.println("[Firebase] OK Ready!");
    lcdPrintLine(0, "Firebase OK!");
    lcdPrintLine(1, "Monitoring...");
  }
  else
  {
    Serial.println("[Firebase] NOT READY!");
    lcdPrintLine(0, "Firebase FAIL");
    lcdPrintLine(1, "Cek Auth/RTDB");
  }

  delay(1000);
  lcd.clear();

  // interval 0.8 detik: baca + kirim
  timer.setInterval(800L, updateSystem);
}

void loop()
{
  Blynk.run();
  timer.run();
}