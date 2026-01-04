#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// --- CONFIGURATION ---
const char* ssid = "Wokwi-GUEST"; 
const char* password = "";
const char* mqtt_server = "host.wokwi.internal"; // Your PC IP

#define DHTPIN 15
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);
WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected!");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP32Sim-";
    clientId += String(random(0xffff), HEX);
    
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5s");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  float t = dht.readTemperature();
  float h=dht.readHumidity();
  if (!isnan(t) && !isnan(h)) {
    String tempStr = String(t);
    String humidStr = String(h);
    client.publish("home/lab1/temp", tempStr.c_str());
    client.publish("home/lab1/humidity", humidStr.c_str());
    Serial.print("Published: ");
    Serial.println(tempStr);
    Serial.println(humidStr);
  }
  delay(2000);
}