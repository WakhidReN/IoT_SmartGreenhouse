// Definisikan Blynk Template ID dan Template Name
#define BLYNK_TEMPLATE_ID "TMPL6BjMDJmAW"                   
#define BLYNK_TEMPLATE_NAME "SmartGreenHouse"               
#define BLYNK_AUTH_TOKEN "AoCzH9dhGzwEjlcy8Ta93NJvuK_w0SmA"

// Definisikan Blynk Print
#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <Adafruit_Sensor.h>
#include "DHT.h"
#include "ThingSpeak.h"

// Definisikan koneksi WiFi
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "JAWIR BERIMAN"; 
char pass[] = "Wisuda2024";   

// deklarasikan data yang dibutuhkan Thingspeak
WiFiClient client;
unsigned long myChannelNumber = 2561454;
const char *myWriteAPIKey = "FUH6QVZOQOCR8M7Q";

DHT dht(2, DHT11);

const int MAX_TEMPERATURE = 32; // Ambang batas suhu untuk menyalakan pompa air
const int PUMP_PIN = 5;         // Pin tempat pompa air dihubungkan

void connectToWiFi();
// void initBlynk();
void bacaDHTdanKontrolPompa();
void kontrolPompa(float suhu);
void sendBlynk();
void sendThingspeak();

void setup()
{
  Serial.begin(9600);
  pinMode(PUMP_PIN, OUTPUT);
  connectToWiFi();
  // initBlynk();
}

void loop()
{
  bacaDHTdanKontrolPompa();
  sendBlynk();      // Mengirim data ke Blynk // Agar Blynk dapat terus berjalan
  sendThingspeak(); // Mengirim data ke ThingSpeak
  delay(2000);      // Jeda 2 detik sebelum membaca lagi
}

void connectToWiFi()
{
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Blynk.begin(auth, ssid, pass);

  // mengubah mode wifi
  WiFi.mode(WIFI_STA);
  // Installasi ThingSpeak
  ThingSpeak.begin(client);
}

void bacaDHTdanKontrolPompa()
{
  float kelembaban = dht.readHumidity();
  float suhu = dht.readTemperature();

  if (isnan(kelembaban) || isnan(suhu))
  {
    Serial.println(F("Gagal Baca Data DHT sensor!"));
  }

  Serial.print("Kelembaban: ");
  Serial.println(kelembaban);
  Serial.print("Suhu: ");
  Serial.println(suhu);

  kontrolPompa(suhu);
}

void kontrolPompa(float suhu)
{
  if (suhu > MAX_TEMPERATURE)
  {
    Serial.println("Suhu temperatur panas !!!");
    digitalWrite(PUMP_PIN, HIGH); // Nyalakan pompa air
  }
  else
  {
    digitalWrite(PUMP_PIN, LOW); // Matikan pompa air
  }
}

void sendBlynk()
{
  Blynk.run();
  float suhu = dht.readTemperature();    // Baca suhu dalam Celcius
  float kelembaban = dht.readHumidity(); // Baca kelembaban

  // Kirim suhu dan kelembaban ke Blynk
  Blynk.virtualWrite(V0, suhu);       // Kirim suhu ke Virtual Pin V0
  Blynk.virtualWrite(V1, kelembaban); // Kirim kelembaban ke Virtual Pin V1

  delay(1000); // Jeda 1 detik sebelum mengirim data lagi
}

// fungsi untuk mengirim data ke Thingspeak
void sendThingspeak()
{
  float suhu = dht.readTemperature();    // Baca suhu dalam Celcius
  float kelembaban = dht.readHumidity(); // Baca kelembaban

  ThingSpeak.setField(1, suhu);
  ThingSpeak.setField(2, kelembaban);
  int Data = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  delay(1000);
}