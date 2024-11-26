// Favoriot RPC [HTTPS] GET
// APIkey = Favoriot APIkey @ deviceAccessToken - either one can use for Favoriot
// but for RPC can only use APIkey

#include <WiFi.h>
#include <HTTPClient.h>
#include <NetworkClientSecure.h>
#include "FavoriotCA.h" // certificate

const char ssid[] = "YOUR_WIFI_SSID";
const char password[] = "YOUR_WIFI_PASS";
const char APIkey[] = "YOUR_APIKEY";

// Change the URL, need to insert your device ID
String serverUrl = "https://apiv2.favoriot.com/v2/rpc?device_developer_id=YOUR_DEVICE_ID&timeout=5000";

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

  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);

  connect_wifi();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connect_wifi();
  }

  // Get data from Platform every 3s
  if (millis() - lastMillis > 3000) {
    lastMillis = millis();

    Serial.println("Getting data from Favoriot ...");
    
    NetworkClientSecure *client = new NetworkClientSecure;

    if(client) {
      client -> setCACert(certificate); {
        HTTPClient https;
      
        https.begin(*client, serverUrl);
      
        https.addHeader("Content-Type", "application/json");
        https.addHeader("Apikey", APIkey);
        
        int httpCode = https.GET();
      
        if(httpCode > 0){
          Serial.print("HTTPS Request: ");
          httpCode == 200? Serial.print("Success, ") : Serial.print("Error, ");

          if (httpCode == 200) {
            String payload = https.getString();
            Serial.println("Received data:");
            Serial.println(payload);

            if (payload.indexOf("\"led\":\"on\"") >= 0) {
              digitalWrite(2, LOW);
              Serial.println("led turned ON");
            } 
            else if (payload.indexOf("\"led\":\"off\"") >= 0) {
              digitalWrite(2, HIGH);
              Serial.println("led turned OFF");
            }

          } 
          else {
            String errorMessage = https.getString();
            Serial.printf("Error: %d\n", httpCode);
            Serial.println("Error Message: " + errorMessage);
          }
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
