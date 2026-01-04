#define BLYNK_TEMPLATE_ID "TMPL6g6bkCK1q"
#define BLYNK_TEMPLATE_NAME "Luggage Tracker"
#define BLYNK_AUTH_TOKEN "_jSSrJ3TPGsEK3Zl-LMcE9PpObaaRkJI"
#define BLYNK_PRINT Serial

#include <Arduino.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/* -------- BLYNK -------- */
char ssid[] = "Abudarda";
char pass[] = "abudarda640";

/* -------- GPS -------- */
TinyGPSPlus gps;
HardwareSerial gpsSerial(2);   // UART2
#define GPS_RX 16
#define GPS_TX 17

/* -------- DHT11 -------- */
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

/* -------- PIR -------- */
#define PIR_PIN 27

/* -------- LDR -------- */
#define LDR_PIN 34   // ADC

/* -------- BUZZER -------- */
#define BUZZER_PIN 26

/* -------- OLED -------- */
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDR 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

/* -------- TIMERS -------- */
BlynkTimer timer;

/* -------- FUNCTIONS -------- */

void readSensors() {
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  int ldr = analogRead(LDR_PIN);
  int motion = digitalRead(PIR_PIN);

  // ---- OLED ----
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);

  display.print("Temp: "); display.print(temp); display.println(" C");
  display.print("Hum : "); display.print(hum); display.println(" %");
  display.print("LDR : "); display.println(ldr);
  display.print("PIR : ");
  display.println(motion ? "Motion" : "No Motion");

  display.display();

  // ---- BUZZER ----
  if (motion) {
    digitalWrite(BUZZER_PIN, HIGH);
    Blynk.virtualWrite(V3, 1);
  } else {
    digitalWrite(BUZZER_PIN, LOW);
    Blynk.virtualWrite(V3, 0);
  }

  // ---- BLYNK ----
  Blynk.virtualWrite(V0, temp);
  Blynk.virtualWrite(V1, hum);
  Blynk.virtualWrite(V2, ldr);

  Serial.println("Sensors Updated");
}

void readGPS() {
  while (gpsSerial.available()) {
    gps.encode(gpsSerial.read());
  }

  if (gps.location.isUpdated()) {
    double lat = gps.location.lat();
    double lng = gps.location.lng();
    int sats = gps.satellites.value();
    double alt = gps.altitude.meters();

    Serial.print("Lat: "); Serial.println(lat, 6);
    Serial.print("Lng: "); Serial.println(lng, 6);
    Serial.print("Sat: "); Serial.println(sats);
    Serial.print("Alt: "); Serial.println(alt);

    Blynk.virtualWrite(V10, lat);
    Blynk.virtualWrite(V11, lng);
    Blynk.virtualWrite(V12, sats);
    Blynk.virtualWrite(V13, alt);
  }
}

/* -------- SETUP -------- */
void setup() {
  Serial.begin(115200);

  pinMode(PIR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  dht.begin();

  gpsSerial.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("OLED failed");
    while (true);
  }

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("System Starting...");
  display.display();

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  timer.setInterval(2000L, readSensors);
  timer.setInterval(1000L, readGPS);

  Serial.println("SYSTEM READY");
}

/* -------- LOOP -------- */
void loop() {
  Blynk.run();
  timer.run();
}
