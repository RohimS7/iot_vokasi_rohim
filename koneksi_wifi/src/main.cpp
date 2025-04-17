#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "DHT.h"

// Definisi pin dan tipe sensor
#define DHTPIN 27    
#define DHTTYPE DHT22  

// Inisialisasi objek DHT
DHT dht(DHTPIN, DHTTYPE);

// Ganti dengan kredensial WiFi Anda
const char* ssid = "tukang misoh";
const char* password = "987654321";

// Variabel untuk interval pengiriman data
unsigned long previousMillis = 0;
const long interval = 5000;  // Interval 5 detik (5000 ms)

void setup() {
  Serial.begin(115200);

  // Hubungkan ke WiFi
  WiFi.begin(ssid, password);
  Serial.print("Menghubungkan ke WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" Terhubung!");

  dht.begin();
  delay(1000); // Tunggu koneksi stabil
}

void loop() {
  unsigned long currentMillis = millis();

  // Lakukan POST setiap interval yang telah ditentukan
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    float humidity = round(dht.readHumidity());
    float temperature = round(dht.readTemperature());

    // Cek apakah pembacaan sensor gagal
    if (isnan(humidity) || isnan(temperature)) {
      Serial.println(F("Gagal membaca dari sensor DHT!"));
      return;
    }

    // Hitung heat index (opsional)
    float heatIndex = dht.computeHeatIndex(temperature, humidity, false);

    // Inisialisasi HTTPClient
    HTTPClient http;
    String url = "http://28a4-175-45-190-3.ngrok-free.app/api/posts"; // Ganti dengan URL ngrok yang kamu punya

    http.begin(url); // Menggunakan HTTP, bukan HTTPS
    http.addHeader("Content-Type", "application/json");

    // Membuat payload JSON
    String payload = "{\"nama_sensor\":\"Sensor GD\", \"nilai1\":" + String(humidity) + ", \"nilai2\":" + String(temperature) + "}";

    Serial.println("Payload:");
    Serial.println(payload);

    // Kirim POST request
    int httpResponseCode = http.POST(payload);

    // Tampilkan kode respons HTTP
    Serial.print("Kode respons HTTP: ");
    Serial.println(httpResponseCode);

    // Tampilkan respons server jika berhasil
    if (httpResponseCode == 200 || httpResponseCode == 201) {
      String response = http.getString();
      Serial.println("Respons dari server:");
      Serial.println(response);
    } else {
      Serial.println("Gagal mengirim data");
    }

    // Tutup koneksi HTTP
    http.end();
  }
}
