# 🌍 BMKG Mini Monitoring System  
### ESP32-S3 + Ultrasonic + Shock Sensor + Blynk + Firebase + Website

---

## 📌 Deskripsi Project

Project ini merupakan sistem monitoring berbasis IoT yang mensimulasikan sistem deteksi gempa sederhana seperti BMKG dalam skala mini menggunakan **ESP32-S3**.

Sistem mampu:
- Mendeteksi **guncangan (shock)**
- Mengukur **jarak (cm)**
- Menampilkan data secara real-time di **LCD**
- Mengirim data ke **Blynk IoT**
- Menyimpan data ke **Firebase Realtime Database**
- Integrasi dengan website (With firebase)

Update sistem dilakukan setiap **0.8 detik**.

---

# 🎯 Tujuan Project

1. Mengembangkan sistem monitoring gempa berbasis IoT
2. Mengintegrasikan sensor fisik dengan cloud
3. Mengimplementasikan komunikasi real-time
4. Menyimpan dan menampilkan data monitoring secara online

---

# ⚙️ Cara Kerja Sistem

1. ESP32 membaca:
   - Sensor Ultrasonic → Mengukur jarak
   - Sensor Shock → Mendeteksi getaran

2. Data ditampilkan pada LCD I2C

3. LED RGB sebagai indikator:
   - 🔴 Merah → Saat terjadi guncangan
   - ⚫ Mati → Saat normal

4. Data dikirim ke:
   - 📱 Blynk (Monitoring Real-Time)
   - ☁ Firebase RTDB (Penyimpanan Data)

5. Website Real-Time: 
   - ☁ Data realtime diambil dari firebase
   -📱 Data ditampilkan diwebsite

6. Proses diulang setiap 800ms (0.8 detik)

---

# 🚀 Fitur Utama

- ✅ Monitoring jarak real-time (cm)
- ✅ Deteksi guncangan (0 / 1)
- ✅ Indikator visual LED RGB
- ✅ Tampilan informasi LCD 16x2
- ✅ Integrasi Blynk IoT
- ✅ Integrasi Firebase RTDB
- ✅ Integrasi Website
- ✅ Logging status koneksi di Serial Monitor
- ✅ Update data setiap 0.8 detik

---

# 🛠 Hardware yang Digunakan

- ESP32-S3
- Sensor Ultrasonic (HC-SR04)
- Sensor Shock / Getaran
- LCD I2C 16x2
- LED RGB
- Resistor (untuk RGB)
- Breadboard & Kabel Jumper

---

# 🔌 Konfigurasi Pin ESP32-S3

| Komponen            | GPIO |
|---------------------|------|
| I2C SDA             | 8    |
| I2C SCL             | 9    |
| Ultrasonic TRIG     | 41   |
| Ultrasonic ECHO     | 38   |
| Shock Sensor        | 21   |
| RGB Red             | 17   |
| RGB Green           | 16   |
| RGB Blue            | 15   |

---

# 🖥 Tampilan LCD

LCD 16x2 digunakan untuk menampilkan informasi utama sistem secara real-time.

## 📍 Kondisi Normal

Saat tidak ada guncangan terdeteksi:
Jarak: --.-- cm
Status: Normal 

Penjelasan:
- Baris pertama menampilkan jarak hasil pembacaan sensor ultrasonic.
- Baris kedua menampilkan status sistem.
- LED RGB dalam kondisi mati.
- Data tetap dikirim ke Blynk dan Firebase.

---

## 🚨 Kondisi Terjadi Gempa

Saat sensor shock mendeteksi getaran:
Jarak: --.-- cm
Status: GEMPAA!!!


Penjelasan:
- Status berubah menjadi **GEMPAA!!!**
- LED RGB menyala merah sebagai indikator bahaya.
- Data guncangan dikirim ke:
  - Blynk (V0 = 1)
  - Firebase (shock = 1)
- Website akan menampilkan status gempa secara real-time.

---

## 🔄 Update Tampilan

LCD diperbarui setiap:
800 ms (0.8 detik)


Setiap update mencakup:
- Pembacaan jarak terbaru
- Status guncangan terbaru
- Perubahan indikator LED

---

## 📊 Informasi Tambahan

Jika terjadi error pembacaan ultrasonic:
Jarak: Err
Status: Normal

Artinya:
- Sensor ultrasonic tidak menerima pantulan echo.
- Sistem tetap berjalan normal.
- Data error tetap dikirim ke cloud.


