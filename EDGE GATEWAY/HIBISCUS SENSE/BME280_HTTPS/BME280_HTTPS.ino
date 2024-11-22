// ESP32 -> Edge Gateway [HTTPS]
// BME280 : altitude, pressure, humidity, temperature
// This code is to send BME280 data from Hibiscus Sense ESP32 to Edge Gateway and manage it using Favoriot
// ESP32 will send device status, altitude, pressure, humidity, temperature using HTTPS protocol

#include <WiFi.h>
#include <HTTPClient.h>
#include <NetworkClientSecure.h>
#include <Adafruit_BME280.h>
#include "FavoriotCA.h" // certificate

const char ssid[] = "YOUR_WIFI_SSID";
const char password[] = "YOUR_WIFI_PASS";
const char APIkey[] = "YOUR_APIKEY";
const char serverUrl[] = "YOUR_URL";

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

  String status = "BME ON";
  float altitude = bme.readAltitude(1015);
  float pressure = bme.readPressure() / 100.00;
  float humidity = bme.readHumidity();
  float temperature = bme.readTemperature();

  // Send data to Platform every 15s
  if (millis() - lastMillis > 15000) {
    lastMillis = millis();

    String payload = "{\"uid\":\"Flame\",\"data\":{";
      payload += "\"status\":\"" + status + "\",";
      payload += "\"temperature\":\"" + String(temperature) + "\",";
      payload += "\"humidity\":\"" + String(humidity) + "\",";
      payload += "\"altitude\":\"" + String(altitude) + "\",";
      payload += "\"pressure\":\"" + String(pressure) + "\"";
    payload += "}}";

    Serial.println("Sending data to Edge Gateway ...");
    Serial.println("Data to Publish: " + payload);
    
    // Sending data to platform - https
    NetworkClientSecure *client = new NetworkClientSecure;

    if(client) {
      client -> setCACert(certificate); {
        HTTPClient https;
      
        https.begin(*client, serverUrl);
      
        https.addHeader("Content-Type", "application/json");
        https.addHeader("Apikey", APIkey);
        
        int httpCode = https.POST(payload);
      
        if(httpCode > 0){
          Serial.print("HTTPS Request: ");
          
          httpCode == 201 ? Serial.print("Success, ") : Serial.print("Error, ");
          Serial.println(https.getString());
        }
        else{
          Serial.println("HTTPS Request Connection Error!");
        }
      
        https.end();
        Serial.println();
      }

      delete client;
    }
    else{
      Serial.println("Unable to create secure client connection!");
      Serial.println();
    }
  }
}
