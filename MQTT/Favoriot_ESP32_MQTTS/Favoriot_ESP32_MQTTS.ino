// Favoriot -> ESP32 [MQTTS]
// RPC

#include <WiFi.h>
#include <MQTT.h>
#include <NetworkClientSecure.h>
#include <ArduinoJson.h>
#include "FavoriotCA.h" // certificate

const char ssid[] = "YOUR_WIFI_SSID";
const char password[] = "YOUR_WIFI_PASS";

const char mqtt_broker[] = "mqtt.favoriot.com";
const int mqtt_port = 8883;
const char mqtt_uniqueID[] = "YOUR_UNIQUE_ID";
const char mqtt_username[] = "YOUR_ACCESS_TOKEN";
const char mqtt_password[] = "YOUR_ACCESS_TOKEN";
const char controlTopic[] = "YOUR_ACCESS_TOKEN/v2/rpc";
const char statusTopic[] = "YOUR_ACCESS_TOKEN/v2/streams/status";

MQTTClient mqtt(4096);
NetworkClientSecure client;

void connect_wifi() {
  Serial.print("Connecting to Wi-Fi: ");
  Serial.print(ssid);
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
  
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, payload);

  if (doc.containsKey("led")) {
    String ledState = doc["led"]; // This retrieves the value "on" or "off"
    if (ledState == "on") {
      Serial.println("LED " + ledState);
      digitalWrite(2, LOW);
    } 
    else {
      Serial.println("LED " + ledState);
      digitalWrite(2, HIGH);
    }
    Serial.println("");
  }
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
  Serial.print("Subscribe to: ");
  Serial.println(String(controlTopic));
  mqtt.subscribe(String(controlTopic));

  Serial.println("");
}

void setup() {
  Serial.begin(115200);

  pinMode(2, OUTPUT);
  digitalWrite(2,HIGH);

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
}
