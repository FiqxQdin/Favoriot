// Combined OTA and Data Sending to Favoriot

#include <WiFi.h>
#include <HTTPClient.h>
#include <Adafruit_BME280.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>

// WiFi credentials
const char ssid[] = "YOUR_WIFI_SSID";
const char password[] = "YOUR_WIFI_PASS";

// Favoriot API credentials
const char deviceDeveloperId[] = "YOUR_DEVICE_ID";
const char APIkey[] = "YOUR_API_KEY";
const char serverUrl[] = "http://apiv2.favoriot.com/v2/streams";


// HTML for OTA login page
const char* loginIndex =
 "<form name='loginForm'>"
 "<table width='20%' bgcolor='A09F9F' align='center'>"
 "<tr>"
 "<td colspan=2>"
 "<center><font size=4><b>ESP32 Login Page</b></font></center>"
 "<br>"
 "</td>"
 "<br>"
 "<br>"
 "</tr>"
 "<tr>"
 "<td>Username:</td>"
 "<td><input type='text' size=25 name='userid'><br></td>"
 "</tr>"
 "<br>"
 "<br>"
 "<tr>"
 "<td>Password:</td>"
 "<td><input type='Password' size=25 name='pwd'><br></td>"
 "<br>"
 "<br>"
 "</tr>"
 "<tr>"
 "<td><input type='submit' onclick='check(this.form)' value='Login'></td>"
 "</tr>"
 "</table>"
 "</form>"
 "<script>"
 "function check(form)"
 "{" 
 "if(form.userid.value=='admin' && form.pwd.value=='admin')"
 "{" 
 "window.open('/serverIndex')"
 "}" 
 "else"
 "{" 
 " alert('Error Password or Username')/*displays error message*/"
 "}" 
 "}" 
 "</script>";

// HTML for OTA server index page
const char* serverIndex =
 "<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>"
 "<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>"
 "<input type='file' name='update'>"
 "<input type='submit' value='Update'>"
 "</form>"
 "<div id='prg'>progress: 0%</div>"
 "<script>"
 "$('form').submit(function(e){"
 "e.preventDefault();"
 "var form = $('#upload_form')[0];"
 "var data = new FormData(form);"
 " $.ajax({"
 "url: '/update',"
 "type: 'POST',"
 "data: data,"
 "contentType: false,"
 "processData:false,"
 "xhr: function() {"
 "var xhr = new window.XMLHttpRequest();"
 "xhr.upload.addEventListener('progress', function(evt) {"
 "if (evt.lengthComputable) {"
 "var per = evt.loaded / evt.total;"
 "$('#prg').html('progress: ' + Math.round(per*100) + '%');"
 "}"
 "}, false);"
 "return xhr;"
 "},"
 "success:function(d, s) {"
 "console.log('success!')"
 "},"
 "error: function (a, b, c) {"
 "}"
 "});"
 "});"
 "</script>";

// OTA parameters
const char* host = "esp32";
WebServer server(80);

Adafruit_BME280 bme;
unsigned long lastMillis = 0;

void setup() {
  Serial.begin(115200);

  // Initialize BME280
  if (!bme.begin()) {
    Serial.println("Failed to find Hibiscus Sense BME280 chip");
  }

  // Connect to WiFi
  connect_wifi();

  // Set up OTA
  setup_OTA();
}

void loop() {
  // Handle OTA requests
  server.handleClient();

  // Send data to Favoriot every 15 seconds
  if (millis() - lastMillis > 15000) {
    lastMillis = millis();

    if (WiFi.status() != WL_CONNECTED) {
      connect_wifi();
    }

    float altitude = bme.readAltitude(1015);
    float pressure = bme.readPressure() / 100.00;
    float humidity = bme.readHumidity();
    float temperature = bme.readTemperature();

    String favoriotJson = "{\"device_developer_id\":\"" + String(deviceDeveloperId) + "\",\"data\":{";
    favoriotJson += "\"altitude\":\"" + String(altitude) + "\",";
    favoriotJson += "\"pressure\":\"" + String(pressure) + "\",";
    favoriotJson += "\"humidity\":\"" + String(humidity) + "\",";
    favoriotJson += "\"temperature\":\"" + String(temperature) + "\"";
    favoriotJson += "}}";

    sendDataToFavoriot(favoriotJson);
  }
}

void connect_wifi() {
  Serial.print("Connecting to Wi-Fi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("\nWi-Fi Connected!");
  Serial.print("ESP32 IP address: ");
  Serial.println(WiFi.localIP());
}

void setup_OTA() {
  if (!MDNS.begin(host)) {
    Serial.println("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");

  server.on("/", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", loginIndex);
  });
  server.on("/serverIndex", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", serverIndex);
  });
  server.on("/update", HTTP_POST, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
  }, []() {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      Serial.printf("Update: %s\n", upload.filename.c_str());
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) {
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
    }
  });
  server.begin();
}

void sendDataToFavoriot(const String& favoriotJson) {
  WiFiClient client;
  HTTPClient http;

  http.begin(client, serverUrl);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Apikey", APIkey);

  int httpCode = http.POST(favoriotJson);

  if (httpCode > 0) {
    Serial.print("HTTP Request: ");
    httpCode == 201 ? Serial.print("Success, ") : Serial.print("Error, ");
    Serial.println(http.getString());
  } else {
    Serial.println("HTTP Request Connection Error!");
  }

  http.end();
}