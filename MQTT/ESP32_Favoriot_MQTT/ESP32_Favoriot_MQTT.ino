// ESP32 -> Favoriot [MQTT]
// BME280 : altitude, pressure, humidity, temperature

#include <WiFi.h>
#include <MQTT.h>
#include <Adafruit_BME280.h>

const char ssid[] = "YOUR_WIFI_SSID";
const char password[] = "YOUR_WIFI_PASS";

const char mqtt_broker[] = "mqtt.favoriot.com";
const int mqtt_port = 1883;
const char mqtt_uniqueID[] = "YOUR_UNIQUE_ID";
const char mqtt_username[] = "YOUR_ACCESS_TOKEN";
const char mqtt_password[] = "YOUR_ACCESS_TOKEN";
const char publishTopic[] = "YOUR_ACCESS_TOKEN/v2/streams";
const char statusTopic[] = "YOUR_ACCESS_TOKEN/v2/streams/status";
const char deviceDeveloperId[] = "YOUR_DEVICE_ID";

Adafruit_BME280 bme;
MQTTClient mqtt(4096);
WiFiClient client;
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

  if (!bme.begin()){
    Serial.println("Failed to find Hibiscus Sense BME280 chip");
  }

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
