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

char ssid[] = "Joanna R";
char pass[] = "yayayaya";

#define I2C_SDA 8
#define I2C_SCL 9
#define TRIG_PIN 39
#define ECHO_PIN 38
#define SHOCK_PIN 21
#define R_PIN 17
#define G_PIN 16
#define B_PIN 15

LiquidCrystal_I2C lcd(0x27, 16, 2);
bool ACTIVE_LOW_SHOCK = false;
BlynkTimer timer;

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

  // LCD tampilkan jarak + status
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

  // Serial Monitor
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.print(" cm | Shock: ");
  Serial.println(shockDetected);

  // Kirim ke Blynk
  if (Blynk.connected())
  {
    Blynk.virtualWrite(V2, distance);
    Blynk.virtualWrite(V3, shockDetected ? 1 : 0);
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

  Wire.begin(I2C_SDA, I2C_SCL);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("BMKG System");
  lcd.setCursor(0, 1);
  lcd.print("Connecting...");

  // Koneksi WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  Serial.print("Connecting WiFi");
  int retry = 0;
  while (WiFi.status() != WL_CONNECTED && retry < 20)
  {
    delay(500);
    Serial.print(".");
    retry++;
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("\nWiFi Connected!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    lcd.setCursor(0, 1);
    lcd.print("WiFi OK!        ");
    delay(800);
  }
  else
  {
    Serial.println("\nWiFi GAGAL!");
    lcd.setCursor(0, 1);
    lcd.print("WiFi GAGAL!     ");
    delay(800);
  }

  // Koneksi Blynk (port 80)
  Blynk.config(BLYNK_AUTH_TOKEN, "blynk.cloud", 80);
  Blynk.connect(5000); // timeout 5 detik

  if (Blynk.connected())
  {
    Serial.println("Blynk Connected!");
    lcd.setCursor(0, 1);
    lcd.print("Blynk OK!       ");
  }
  else
  {
    Serial.println("Blynk GAGAL - cek token/template");
    lcd.setCursor(0, 1);
    lcd.print("Blynk Error...  ");
  }

  delay(1000);
  lcd.clear();

  timer.setInterval(1000L, updateSystem);
}

void loop()
{
  Blynk.run();
  timer.run();
}