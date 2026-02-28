#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>

// ================= WIFI =================
#define WIFI_SSID "iPhone"
#define WIFI_PASSWORD "12341234"

// ================= FIREBASE =================
#define API_KEY "AIzaSyC0Qb4LAbcQWe-2DXLMueUo3uWr1RKNq9Q"
#define DATABASE_URL "https://ca-week3-default-rtdb.firebaseio.com"
#define USER_EMAIL "rifki.dupon07@gmail.com"
#define USER_PASSWORD "Rifkiwidya6*"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

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

// ================= Firebase init =================
void initFirebase()
{
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  Serial.println("Firebase init called");
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
  initFirebase();
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

  // ================= FIREBASE UPDATE =================
  if (millis() - lastSend > sendInterval)
  {
    lastSend = millis();

    if (WiFi.status() != WL_CONNECTED)
    {
      Serial.println("❌ WiFi not connected, skip Firebase");
      return;
    }

    if (!Firebase.ready())
    {
      Serial.println("⏳ Firebase not ready yet, skip");
      return;
    }

    bool ok1 = Firebase.RTDB.setFloat(&fbdo, "/BMKG/distance_cm", distance);
    bool ok2 = Firebase.RTDB.setBool(&fbdo, "/BMKG/shock_detected", shockDetected);
    bool ok3 = Firebase.RTDB.setBool(&fbdo, "/BMKG/led_red", ledStatus);
    bool ok4 = Firebase.RTDB.setBool(&fbdo, "/BMKG/lcd_status", lcdOK);

    if (ok1 && ok2 && ok3 && ok4)
    {
      Serial.println("✅ Data BERHASIL dikirim ke Firebase");
    }
    else
    {
      Serial.println("❌ GAGAL kirim ke Firebase");
      Serial.print("Error: ");
      Serial.println(fbdo.errorReason());
    }
  }

  delay(200);
}