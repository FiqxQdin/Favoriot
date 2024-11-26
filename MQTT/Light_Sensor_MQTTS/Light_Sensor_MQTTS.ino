// ESP32 -> Favoriot [MQTTS]
// Photoresistor @ Light Sensor

#include <WiFi.h>
#include <MQTT.h>
#include <NetworkClientSecure.h>
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

MQTTClient mqtt(4096);
NetworkClientSecure client;

const int sensor_pin = 32;
const float max_value = 4095;  
const float max_volt = 3.3;   
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

  pinMode(sensor_pin, INPUT);

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

  int raw_value = analogRead(sensor_pin); 
  float volt_value = (raw_value / max_value) * max_volt; 

  // Send data to Platform every 15s
  if (millis() - lastMillis > 15000) {
    lastMillis = millis();

    String favoriotJson = "{\"device_developer_id\":\"" + String(deviceDeveloperId) + "\",\"data\":{";
      favoriotJson += "\"raw\":\"" + String(raw_value) + "\",";
      favoriotJson += "\"voltage\":\"" + String(volt_value) + "\"";
    favoriotJson += "}}";

    Serial.println("Sending data to Favoriot's Data Stream ...");
    Serial.println("Data to Publish: " + favoriotJson);
    Serial.println("Publish to: " + String(publishTopic));
    mqtt.publish(String(publishTopic), favoriotJson);
    Serial.println("");
  }

  delay(3000);
}
