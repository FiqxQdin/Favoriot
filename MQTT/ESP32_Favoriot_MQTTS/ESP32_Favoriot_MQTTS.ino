// ESP32 -> Favoriot [MQTTS]
// BME280 : altitude, pressure, humidity, temperature

#include <WiFi.h>
#include <MQTT.h>
#include <NetworkClientSecure.h>
#include <Adafruit_BME280.h>
#include "FavoriotCA.h" // certificate

const char ssid[] = "favoriot@unifi";
const char password[] = "fav0r10t2017";

const char mqtt_broker[] = "mqtt.favoriot.com";
const int mqtt_port = 8883;
const char mqtt_uniqueID[] = "FIQXQDIN";
const char mqtt_username[] = "1Ydpudja2l1YGz6jDnmloMBynvOckVpg";
const char mqtt_password[] = "1Ydpudja2l1YGz6jDnmloMBynvOckVpg";
const char publishTopic[] = "1Ydpudja2l1YGz6jDnmloMBynvOckVpg/v2/streams";
const char statusTopic[] = "1Ydpudja2l1YGz6jDnmloMBynvOckVpg/v2/streams/status";
const char deviceDeveloperId[] = "Hibiscus_Sense_ESP32@fiqxqdin";

Adafruit_BME280 bme;
MQTTClient mqtt(4096);
NetworkClientSecure client;
unsigned long lastMillis = 0;

void connect_wifi() {
  Serial.print("Connecting to Wi-Fi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while(WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("Wi-Fi Connected!");
  Serial.print("ESP32 IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("");
}

void messageReceived(String &topic, String &payload) {
  Serial.println("Incoming: " + topic + " - " + payload);
  Serial.println();
}

void connect_mqtts() {
  Serial.print("Connecting to MQTT Broker ...");
  client.setCACert(certificate); // Certificate
  mqtt.begin(mqtt_broker, mqtt_port, client);
  mqtt.onMessage(messageReceived); // Incoming message

  while (!mqtt.connect(mqtt_uniqueID, mqtt_username, mqtt_password)) {
    Serial.print(".");
    delay(500);
  }

  Serial.println(" connected!");

  Serial.print("Subscribe to: ");
  Serial.println(String(statusTopic));
  mqtt.subscribe(String(statusTopic));
  Serial.println();
}

void setup() {
  Serial.begin(115200);

  if (!bme.begin()){
    Serial.println("Failed to find Hibiscus Sense BME280 chip");
  }

  connect_wifi();
  connect_mqtts();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connect_wifi();
  }

  if (!mqtt.connected()) {
    connect_mqtts();
  }
  mqtt.loop();
  delay(10);  // <- fixes some issues with WiFi stability

  float altitude = bme.readAltitude(1015);
  float pressure = bme.readPressure() / 100.00;
  float humidity = bme.readHumidity();
  float temperature = bme.readTemperature();

  // Send data to Platform every 15s
  if (millis() - lastMillis > 15000) {
    lastMillis = millis();

    String favoriotJson = "{\"device_developer_id\":\"" + String(deviceDeveloperId) + "\",\"data\":{";
      favoriotJson += "\"altitude\":\"" + String(altitude) + "\",";
      favoriotJson += "\"pressure\":\"" + String(pressure) + "\",";
      favoriotJson += "\"humidity\":\"" + String(humidity) + "\",";
      favoriotJson += "\"temperature\":\"" + String(temperature) + "\"";
    favoriotJson += "}}";

    Serial.println("Sending data to Favoriot's Data Stream ...");
    Serial.println("Data to Publish: " + favoriotJson);
    Serial.println("Publish to: " + String(publishTopic));
    mqtt.publish(String(publishTopic), favoriotJson);
  }

  delay(3000);
}
