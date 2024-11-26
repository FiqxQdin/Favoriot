// ESP32 -> Favoriot [HTTPS]
// BME280 : altitude, pressure, humidity, temperature

#include <WiFi.h>
#include <HTTPClient.h>
#include <NetworkClientSecure.h>
#include <Adafruit_BME280.h>
#include "FavoriotCA.h" // certificate

const char ssid[] = "YOUR_WIFI_SSID";
const char password[] = "YOUR_WIFI_PASS";
const char deviceDeveloperId[] = "YOUR_DEVICE_ID";
const char APIkey[] = "YOUR_APIKEY";
const char serverUrl[] = "https://apiv2.favoriot.com/v2/streams";

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
    
    // Sending data to platform - https
    NetworkClientSecure *client = new NetworkClientSecure;

    if(client) {
      client -> setCACert(certificate); {
        HTTPClient https;
      
        https.begin(*client, serverUrl);
      
        https.addHeader("Content-Type", "application/json");

        // APIkey @ deviceAccessToken - either one for Favoriot
        https.addHeader("Apikey", APIkey);
        // https.addHeader("Apikey", deviceAccessToken);
        
        int httpCode = https.POST(favoriotJson);
      
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
