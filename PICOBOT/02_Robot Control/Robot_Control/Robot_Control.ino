#include <WiFi.h>
#include <MQTT.h>
#include <ArduinoJson.h>

// Define motor control pins
#define PWM_M1A 8  // Left Motor 1 Forward
#define PWM_M1B 9  // Left Motor 1 Backward
#define PWM_M2A 10 // Right Motor 2 Forward
#define PWM_M2B 11 // Right Motor 2 Backward

int motorSpeed = 128; // Speed of the motor * max = 255

// Wi-Fi Properties
const char ssid[] = "YOUR_WIFI_SSID";
const char password[] = "YOUR_WIFI_PASS";

// MQTT Properties
const char mqtt_broker[] = "mqtt.favoriot.com";
const int mqtt_port = 1883;
const char mqtt_uniqueID[] = "YOUR_USERNAME";
const char mqtt_username[] = "YOUR_ACCESS_TOKEN";
const char mqtt_password[] = "YOUR_ACCESS_TOKEN";
const char controlTopic[] = "YOUR_ACCESS_TOKEN/v2/rpc";
const char statusTopic[] = "YOUR_ACCESS_TOKEN/v2/streams/status";

MQTTClient mqtt(4096);
WiFiClient client;
String currentMode = "auto";

// Function to connect to Wi-Fi
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

// Callback Function for MQTT
void messageReceived(String &topic, String &payload) {
  Serial.println("Incoming: " + topic + " - " + payload);

  // Check for mode change commands
  if (payload.indexOf("\"manual\":\"on\"") >= 0) {
    currentMode = "manual";
    Serial.println("Mode : MANUAL");
  } else if (payload.indexOf("\"auto\":\"on\"") >= 0) {
    currentMode = "auto";
    Serial.println("Mode : AUTO");
  } else if (payload.indexOf("\"line\":\"on\"") >= 0) {
    currentMode = "line";
    Serial.println("Mode : LINE FOLLOWER");
  }

  if (currentMode == "manual") {
    if (payload.indexOf("\"move\":\"forward\"") >= 0) {
      move_forward(motorSpeed);
    } else if (payload.indexOf("\"move\":\"stop\"") >= 0) {
      stop();
    } else if (payload.indexOf("\"move\":\"reverse\"") >= 0) {
      reverse(motorSpeed);
    } else if (payload.indexOf("\"turn\":\"right\"") >= 0) {
      turn_right(motorSpeed);
    } else if (payload.indexOf("\"turn\":\"left\"") >= 0) {
      turn_left(motorSpeed);
    }  
  }

  Serial.println("");
}


// Function to connect with MQTT
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
  Serial.print("Subscribe to: ");
  Serial.println(String(controlTopic));
  mqtt.subscribe(String(controlTopic));

  Serial.println("");
}

// Function to set motor throttle (speed and direction)
// Throttle range: -255 to +255
void setMotor(int pinA, int pinB, int throttle) {
  if (throttle > 0) {
    analogWrite(pinA, throttle); // Forward
    analogWrite(pinB, 0);
  } else if (throttle < 0) {
    analogWrite(pinA, 0);
    analogWrite(pinB, -throttle); // Backward
  } else {
    analogWrite(pinA, 0); // Stop
    analogWrite(pinB, 0);
  }
}

// Function of 2 wheeled differential drive
void move_forward(int speed) {
  Serial.println("\nForwards");
  setMotor(PWM_M1A, PWM_M1B, speed); // Motor 1 forward * 255 = full speed
  setMotor(PWM_M2A, PWM_M2B, speed); // Motor 2 forward * 128 = half speed
}
void reverse(int speed) {
  Serial.println("\nBackwards");
  setMotor(PWM_M1A, PWM_M1B, -speed); // Motor 1 backward 
  setMotor(PWM_M2A, PWM_M2B, -speed); // Motor 2 backward 
}
void turn_right(int speed) {
  setMotor(PWM_M1A, PWM_M1B, speed); // Motor 1 forward
  setMotor(PWM_M2A, PWM_M2B, 0); // Motor 2 stop
}
void turn_left(int speed) {
  setMotor(PWM_M1A, PWM_M1B, 0); // Motor 1 stop
  setMotor(PWM_M2A, PWM_M2B, speed); // Motor 2 forward
}
void stop() {
  setMotor(PWM_M1A, PWM_M1B, 0); // Motor 1 stop
  setMotor(PWM_M2A, PWM_M2B, 0); // Motor 2 stop
}

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(115200);

  // Set motor control pins as output
  pinMode(PWM_M1A, OUTPUT);
  pinMode(PWM_M1B, OUTPUT);
  pinMode(PWM_M2A, OUTPUT);
  pinMode(PWM_M2B, OUTPUT);

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
}