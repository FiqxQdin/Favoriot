#include <WiFi.h>
#include <MQTT.h>

// Define motor control pins
#define PWM_M1A 8  // Left Motor 1 Forward
#define PWM_M1B 9  // Left Motor 1 Backward
#define PWM_M2A 10 // Right Motor 2 Forward
#define PWM_M2B 11 // Right Motor 2 Backward

// Wi-Fi Details
const char ssid[] = "YOUR_WIFI_SSID";
const char password[] = "YOUR_WIFI_PASS";

// MQTT Details
const char mqtt_broker[] = "mqtt.favoriot.com";
const int mqtt_port = 1883;
const char mqtt_uniqueID[] = "YOUR_USERNAME";
const char mqtt_username[] = "YOUR_ACCESS_TOKEN";
const char mqtt_password[] = "YOUR_ACCESS_TOKEN";
const char publishTopic[] = "YOUR_ACCESS_TOKEN/v2/streams";
const char statusTopic[] = "YOUR_ACCESS_TOKEN/v2/streams/status";
const char deviceDeveloperId[] = "YOUR_DEVICE_ID";

// Light Sensor Details
const int sensor_pin = 27;     // Light sensor pin = GP27
const float max_value = 1023;  // 10-bit ADC
const float max_volt = 3.3;    // Max voltage

// Variable
unsigned long lastMillis = 0;
int motorSpeed = 128; // Speed of the motor * max = 255

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
  Serial.begin(115200);

  pinMode(sensor_pin, INPUT);

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

  int raw_value = analogRead(sensor_pin); 
  float volt_value = (raw_value / max_value) * max_volt; 

  // Light searching
  if (volt_value < 0.9) { 
    move_forward(128);
    Serial.println("Move Forward");
  } else {
    turn_right(64);
    Serial.println("Turn Right");
  }

  // Send data to Platform every 1s
  if (millis() - lastMillis > 1000) {
    lastMillis = millis();

    String favoriotJson = "{\"device_developer_id\":\"" + String(deviceDeveloperId) + "\",\"data\":{";
      favoriotJson += "\"raw\":\"" + String(raw_value) + "\",";
      favoriotJson += "\"voltage\":\"" + String(volt_value) + "\"";
    favoriotJson += "}}";

    Serial.println("Sending data to Favoriot's Data Stream ...");
    Serial.println("Data to Publish: " + favoriotJson);
    Serial.println("Publish to: " + String(publishTopic));
    mqtt.publish(String(publishTopic), favoriotJson);
    Serial.println("");
  }

  // delay(2000);
}