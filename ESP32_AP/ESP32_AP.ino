/* Description : 
  In summary, this code does the following:
  1. Sets up the ESP32 as a web server to collect Wi-Fi credentials, an API key, and a device ID via an HTML form.
  2. Saves these inputs in non-volatile storage (NVS) on the ESP32.
  3. Attempts to connect to the provided Wi-Fi network on startup. 
    If no saved credentials are found, it sets up an access point (AP) to allow user input.
  4. Deletes the saved data when a specific button (connected to pin 0) is pressed and restarts the ESP32.
  5. Reads sensor data from the Hibiscus Sense BME280 chip and sends it to the Favoriot platform at regular intervals.
*/

#include <WiFi.h>          // Include the WiFi library
#include <WebServer.h>     // Include the WebServer library
#include <Preferences.h>   // Include the Preferences library to save input
#include <HTTPClient.h>    // Include the HTTPClient library for HTTP requests
#include <Adafruit_BME280.h> // Included the Adafruit BME280 library

// Define Wi-Fi credentials for the ESP32 Access Point
const char* ssid = "ESP32 FIQXQDIN";  // Access Point Name
const char* password = "FIQXQDIN01";  // Access Point Password
const char serverUrl[] = "http://apiv2.favoriot.com/v2/streams";   // Defined the server URL

int pb = 0;  // Define the pin number for the push button
unsigned long lastMillis = 0;   // 

// Variables to store user inputs
String inputSSID = "";
String inputPassword = "";
String apiKey = "";
String deviceId = "";

WebServer server(80);       // Create a web server instance on port 80
Preferences preferences;    // Create a Preferences object for NVS storage
Adafruit_BME280 bme;

// Function to handle the root route "/"
void handleRoot() {
  // Send an HTML form to collect user inputs
  String html = "<h1>ESP32 Configuration</h1>";
    html += "<form action='/submit' method='POST'>";
    html += "<label style='font-weight:bold;'>Wi-Fi SSID:</label> <input type='text' name='ssid'><br>";
    html += "<label style='font-weight:bold;'>Wi-Fi Password:</label> <input type='password' name='password'><br>";
    html += "<label style='font-weight:bold;'>API Key:</label> <input type='text' name='apikey'><br>";
    html += "<label style='font-weight:bold;'>Device Developer ID:</label> <input type='text' name='deviceid'><br>";
    html += "<input type='submit' value='Submit' style='background-color:blue; color:white;'><br>";
  html += "</form>";

  server.send(200, "text/html", html);  // Send the HTML form as a response
}

// Function to handle the root route "/submit"
void handleFormSubmit() {
  // Capture the form data
  inputSSID = server.arg("ssid");
  inputPassword = server.arg("password");
  apiKey = server.arg("apikey");
  deviceId = server.arg("deviceid");

  // Save the data to flash memory using Preferences
  preferences.begin("config", false); // Open a namespace called "config"
  preferences.putString("ssid", inputSSID);
  preferences.putString("password", inputPassword);
  preferences.putString("apikey", apiKey);
  preferences.putString("deviceid", deviceId);
  preferences.end(); // Close the preferences storage

  // Send a response confirming the submission
  String response = "<h1>Form Submitted Successfully</h1>";
    response += "<p><strong>Wi-Fi SSID:</strong><br>" + inputSSID + "</p>";
    response += "<p><strong>Wi-Fi Password:</strong><br>" + inputPassword + "</p>";
    response += "<p><strong>API Key:</strong><br>" + apiKey + "</p>";
    response += "<p><strong>Device Developer ID:</strong><br>" + deviceId + "</p>";
  response += "<p>Data saved to ESP32 memory!</p>";
  server.send(200, "text/html", response);  // Send the response
}

// Function to load saved data on startup
void loadSavedData() {
  preferences.begin("config", true); // Open in read-only mode
  inputSSID = preferences.getString("ssid", ""); // Default to empty string if not found
  inputPassword = preferences.getString("password", "");
  apiKey = preferences.getString("apikey", "");
  deviceId = preferences.getString("deviceid", "");
  preferences.end(); // Close the preferences storage

  // Print the loaded data to the Serial Monitor
  Serial.println("Loaded saved data:");
  Serial.println("Wi-Fi SSID: " + inputSSID);
  Serial.println("Wi-Fi Password: " + inputPassword);
  Serial.println("API Key: " + apiKey);
  Serial.println("Device Developer ID: " + deviceId);
}

// Function to connect to Wi-Fi
void connect_wifi(const char* ssid, const char* password) {
  Serial.print("Connecting to Wi-Fi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);  // Start Wi-Fi connection

  while (WiFi.status() != WL_CONNECTED) { // Wait until connected
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("Wi-Fi Connected!");
  Serial.print("ESP32 IP address: ");
  Serial.println(WiFi.localIP());  // Print the IP address
  Serial.println("");
}

// Function to set ESP32 as AP
void AP_mode() {
  // Start the ESP32 in Access Point mode
  WiFi.softAP(ssid, password);
  Serial.print("Wi-Fi SSID: ");
  Serial.println(ssid);
  Serial.print("Wi-Fi PASSWORD: ");
  Serial.println(password);

  // Print the IP address of the AP
  IPAddress IP = WiFi.softAPIP();
  Serial.print("Access Point IP: ");
  Serial.println(IP);

  // Set up the routes for the web server
  server.on("/", handleRoot);          // Handle root route (form page)
  server.on("/submit", HTTP_POST, handleFormSubmit);  // Handle form submission

  // Start the web server
  server.begin();
  Serial.println("HTTP server started");
}

// Function to delete saved data
void deleteSavedData() {
  preferences.begin("config", false); // Open a namespace called "config"
  preferences.clear(); // Clear all preferences in this namespace
  preferences.end(); // Close the preferences storage

  // Print a confirmation to the Serial Monitor
  Serial.println("Saved data cleared.");
}

void setup() {
  Serial.begin(115200);  // Initialize serial communication at 115200 baud rate

  pinMode(0, INPUT);  // Configure pin 0 as an input
  if (!bme.begin()){
    Serial.println("Failed to find Hibiscus Sense BME280 chip");
  }

  // Load any previously saved data
  loadSavedData();
  if (inputSSID == "") {  // If no saved SSID, start AP mode
    AP_mode();
  } 
  else {  // If SSID is available, connect to Wi-Fi
    connect_wifi(inputSSID.c_str(), inputPassword.c_str());
  }
}

void loop() {
  // Handle incoming client requests
  server.handleClient();

  int pbstatus = digitalRead(pb);  // Read the status of the push button
  if(pbstatus == LOW){  // If button is pressed
    deleteSavedData();  // Delete saved data
    ESP.restart();
  }

  float altitude = bme.readAltitude(1015);
  float pressure = bme.readPressure() / 100.00;
  float humidity = bme.readHumidity();
  float temperature = bme.readTemperature();

  if (inputSSID != "") {  // If no saved SSID, start AP mode
    if (WiFi.status() != WL_CONNECTED) {
      connect_wifi(inputSSID.c_str(), inputPassword.c_str());
    }

    if (millis() - lastMillis > 5000) {
      lastMillis = millis();

      String favoriotJson = "{\"device_developer_id\":\"" + String(deviceId) + "\",\"data\":{";
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
      http.addHeader("Apikey", apiKey);

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
}
