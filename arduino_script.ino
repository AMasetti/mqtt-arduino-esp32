#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

#define SSID "my-ssid"
#define PASSWORD "12345678"
#define DHTPIN 2
#define DHTTYPE DHT11
#define MQTT_SERVER "broker.hivemq.com"
#define MQTT_PORT 1883
#define MQTT_TOPIC "eko-ai-script/data"

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);

  delay(1000);

  WiFi.begin(SSID, PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");

  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);

  while (!mqttClient.connected()) {
    Serial.println("Connecting to MQTT...");

    if (mqttClient.connect("ESP32Client")) {
      Serial.println("Connected to MQTT");
    } else {
      Serial.print("Failed to connect to MQTT, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" retrying...");
      delay(1000);
    }
  }
}

void loop() {
  delay(2000);

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT");
    return;
  }

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print("% ");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println("*C");

  char payload[20];
  sprintf(payload, "%.1f,%.01f", h, t);

  mqttClient.publish(MQTT_TOPIC, payload);
}
