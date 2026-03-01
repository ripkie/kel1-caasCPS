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

char ssid[] = "iPhone";
char pass[] = "12341234";

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

  // LED merah saat shock
  digitalWrite(R_PIN, shockDetected ? HIGH : LOW);
  digitalWrite(G_PIN, LOW);
  digitalWrite(B_PIN, LOW);

  // status lcd sir
  bool wifiOK = (WiFi.status() == WL_CONNECTED);
  bool blynkOK = Blynk.connected();

  String line0 = "Jarak:";
  if (distance < 0)
    line0 += "Err";
  else
  {
    line0 += String(distance, 1);
    line0 += " cm";
  }
  /*line0 += " ";
  line0 += wifiOK ? "W" : "w";
  line0 += blynkOK ? "B" : "b"; */

  String line1 = "Status: ";
  line1 += shockDetected ? "GEMPAA!!!" : "Normal";
  // line1 += " V0:";
  // line1 += shockDetected ? "1" : "0";

  lcdPrintLine(0, line0);
  lcdPrintLine(1, line1);

  // Kirim ke blynk
  if (blynkOK)
  {
    Blynk.virtualWrite(V0, shockDetected ? 1 : 0); // V0 indikator gempa sir
    Blynk.virtualWrite(V1, distance);              // V1 jarak

    Serial.print("[SEND OK] Shock=");
    Serial.print(shockDetected ? 1 : 0);
    Serial.print(" Jarak=");
    Serial.print(distance);
    Serial.println("cm");
  }
  else
  {
    Serial.println("[SEND SKIP] Blynk not connected");
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
    lcdPrintLine(1, "Monitoring...");
  }
  else
  {
    Serial.println("[Blynk] GAGAL!");
    lcdPrintLine(0, "Blynk GAGAL!");
    lcdPrintLine(1, "Cek token");
  }
  delay(1000);
  lcd.clear();

  timer.setInterval(800L, updateSystem);
}

void loop()
{
  Blynk.run();
  timer.run();
}