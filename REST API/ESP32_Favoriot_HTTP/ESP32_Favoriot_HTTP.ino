// ESP32 -> Favoriot [HTTP]
// BME280 : altitude, pressure, humidity, temperature

#include <WiFi.h>
#include <HTTPClient.h>
#include <Adafruit_BME280.h>

const char ssid[] = "YOUR_WIFI_SSID";
const char password[] = "YOUR_WIFI_PASS";
const char deviceDeveloperId[] = "YOUR_DEVICE_ID";
const char APIkey[] = "YOUR_FAVORIOT_APIKEY";
const char serverUrl[] = "http://apiv2.favoriot.com/v2/streams";

Adafruit_BME280 bme;
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

void setup() {
  Serial.begin(115200);

  if (!bme.begin()){
    Serial.println("Failed to find Hibiscus Sense BME280 chip");
  }

  connect_wifi();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connect_wifi();
  }

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
    
    // Sending data to platform - http
    WiFiClient client;

    HTTPClient http;

    http.begin(client, serverUrl);

    http.addHeader("Content-Type", "application/json");
    http.addHeader("Apikey", APIkey);

    int httpCode = http.POST(favoriotJson);

    if(httpCode > 0){
      Serial.print("HTTP Request: ");
          
      httpCode == 201 ? Serial.print("Success, ") : Serial.print("Error, ");
      Serial.println(http.getString());
    }
    else{
      Serial.println("HTTP Request Connection Error!");
    }

    http.end();
    Serial.println();
  }
}
