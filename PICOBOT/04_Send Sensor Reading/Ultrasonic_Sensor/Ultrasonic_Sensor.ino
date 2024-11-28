#include <WiFi.h>
#include <MQTT.h>
#include <NewPing.h>

// Ultrasonic sensor pins
#define TRIGGER_PIN 16
#define ECHO_PIN 17
#define MAX_DISTANCE 200 // Maximum distance to measure (in cm)

const char ssid[] = "YOUR_WIFI_SSID";
const char password[] = "YOUR_WIFI_PASS";

const char mqtt_broker[] = "mqtt.favoriot.com";
const int mqtt_port = 1883;
const char mqtt_uniqueID[] = "YOUR_USERNAME";
const char mqtt_username[] = "YOUR_ACCESS_TOKEN";
const char mqtt_password[] = "YOUR_ACCESS_TOKEN";
const char publishTopic[] = "YOUR_ACCESS_TOKEN/v2/streams";
const char statusTopic[] = "YOUR_ACCESS_TOKEN/v2/streams/status";
const char deviceDeveloperId[] = "YOUR_DEVICE_ID";

unsigned long lastMillis = 0;

// Ultrasonic sensor object
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
MQTTClient mqtt(4096);
WiFiClient client;

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

void connect_mqtt() {
  Serial.print("Connecting to MQTT Broker ...");
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

  connect_wifi();
  connect_mqtt();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connect_wifi();
  }

  if (!mqtt.connected()) {
    connect_mqtt();
  }
  mqtt.loop();
  delay(10);  // <- fixes some issues with WiFi stability

  // Read distance from ultrasonic sensor
  unsigned int distance = sonar.ping_cm();

  // Debug output
  if (distance == 0) {
    Serial.println("Object out of range. Max range = 200 cm.");
  } else {
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");
  }

  // Send data to Platform every 1s
  if (millis() - lastMillis > 10000) {
    lastMillis = millis();

    String favoriotJson = "{\"device_developer_id\":\"" + String(deviceDeveloperId) + "\",\"data\":{";
      favoriotJson += "\"distance\":\"" + String(distance) + "\"";
    favoriotJson += "}}";

    Serial.println("Sending data to Favoriot's Data Stream ...");
    Serial.println("Data to Publish: " + favoriotJson);
    Serial.println("Publish to: " + String(publishTopic));
    mqtt.publish(String(publishTopic), favoriotJson);
    Serial.println("");
  }

  delay(100); // Allow sensor to stabilize

}
