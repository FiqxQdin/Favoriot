// Include Library
#include <WiFi.h>
#include <MQTT.h>
#include <DHT.h>
#include <NewPing.h>

#define DHTPIN 5          // Pin connected to the DHT11 sensor (GPIO 5)
#define DHTTYPE DHT11     // Define the sensor type as DHT11
#define TRIGGER_PIN 16    // Ultrasonic sensor pins
#define ECHO_PIN 17
#define MAX_DISTANCE 200  // Maximum distance to measure (in cm)
#define PWM_M1A 8  // Left Motor 1 Forward // Define motor control pins
#define PWM_M1B 9  // Left Motor 1 Backward
#define PWM_M2A 10 // Right Motor 2 Forward
#define PWM_M2B 11 // Right Motor 2 Backward

// Constant Variable
// Wi-Fi Properties
const char ssid[] = "YOUR_WIFI_SSID";              
const char password[] = "YOUR_WIFI_PASS";
// MQTT Properties
const char mqtt_broker[] = "mqtt.favoriot.com"; 
const int mqtt_port = 1883;
const char mqtt_uniqueID[] = "YOUR_USERNAME";
const char mqtt_username[] = "YOUR_ACCESS_TOKEN";
const char mqtt_password[] = "YOUR_ACCESS_TOKEN";
const char publishTopic[] = "YOUR_ACCESS_TOKEN/v2/streams";
const char statusTopic[] = "YOUR_ACCESS_TOKEN/v2/streams/status";
const char controlTopic[] = "YOUR_ACCESS_TOKEN/v2/rpc";
const char deviceDeveloperId[] = "YOUR_DEVICE_ID";
// Light Sensor Properties
const int sensor_pin = 27;     // Light sensor pin = GP27
const int max_value = 1023;  // 10-bit ADC
const float max_volt = 3.3;    // Max voltage
// Air Quality Pin
const int airQualityPin = 28;
// Line Sensor Pin
const int line_pin = 26;

// Variable
unsigned long lastMillis = 0;
struct SensorData { 
  float light;
  float humidity;
  float temperature;
  float airQuality;
  float distance;
  float line;
};
int motorSpeed = 200; // Speed of the motor * max = 255
String currentMode;
String loop_payload;

// Object @ Class
MQTTClient mqtt(4096);
WiFiClient client;
DHT dht(DHTPIN, DHTTYPE);  // Create an instance of the DHT class
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

// Function to connect with Wi-Fi
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
  Serial.print("Pico IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("");


}

// Function RPC received message
void messageReceived(String &topic, String &payload) {
  Serial.println("Incoming: " + topic + " - " + payload);

  loop_payload = payload;

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

  Serial.println();
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
  Serial.println();
}

// Function to read sensor data
SensorData sensor_reading() {
  SensorData data;

  // Light Sensor Reading
  float raw_light = analogRead(sensor_pin);
  data.light = (raw_light / max_value) * max_volt;
  // DHT11 Reading
  data.humidity = dht.readHumidity();
  data.temperature = dht.readTemperature();
  // Air Quality
  float raw_airQuality = analogRead(airQualityPin);
  data.airQuality = raw_airQuality * (3.3 / 1023);
  // Ultrasonic Reading
  data.distance = sonar.ping_cm();
  // Maker Line Reading
  int sensorValue = analogRead(line_pin);
  data.line = (sensorValue * 3.3) / 1023.0;

  return data;
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

// Setup Function
void setup() {
  Serial.begin(115200);

  pinMode(sensor_pin, INPUT);

  dht.begin();

  // Set motor control pins as output
  pinMode(PWM_M1A, OUTPUT);
  pinMode(PWM_M1B, OUTPUT);
  pinMode(PWM_M2A, OUTPUT);
  pinMode(PWM_M2B, OUTPUT);

  connect_wifi();
  connect_mqtt();
}

// Loop Function
void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connect_wifi();
  }

  if (!mqtt.connected()) {
    connect_mqtt();
  }
  mqtt.loop();
  delay(10);  // <- fixes some issues with WiFi stability
  
  // Read Sensor Data
  SensorData data = sensor_reading();
  bool obstacleDetected = (data.distance > 0 && data.distance <= 10);
  bool obstacleAvoidanceState = false;

  // Send data to Platform every 5s
  if (millis() - lastMillis > 5000) {
    lastMillis = millis();

    String favoriotJson = "{\"device_developer_id\":\"" + String(deviceDeveloperId) + "\",\"data\":{";
      favoriotJson += "\"light_sensor\":\"" + String(data.light) + "\",";
      favoriotJson += "\"distance\":\"" + String(data.distance) + "\",";
      favoriotJson += "\"air_quality\":\"" + String(data.airQuality) + "\",";
      favoriotJson += "\"temperature\":\"" + String(data.temperature) + "\",";
      favoriotJson += "\"humidity\":\"" + String(data.humidity) + "\"";
    favoriotJson += "}}";

    Serial.println("Sending data to Favoriot's Data Stream ...");
    Serial.println("Data to Publish: " + favoriotJson);
    Serial.println("Publish to: " + String(publishTopic));
    mqtt.publish(String(publishTopic), favoriotJson);
    Serial.println("");
  }

  // Run PicoBot by Mode
  if (currentMode == "manual") {
    if (loop_payload.indexOf("\"move\":\"forward\"") >= 0) {
      move_forward(motorSpeed);
    } else if (loop_payload.indexOf("\"move\":\"stop\"") >= 0) {
      stop();
    } else if (loop_payload.indexOf("\"move\":\"reverse\"") >= 0) {
      reverse(motorSpeed);
    } else if (loop_payload.indexOf("\"turn\":\"right\"") >= 0) {
      turn_right(motorSpeed);
    } else if (loop_payload.indexOf("\"turn\":\"left\"") >= 0) {
      turn_left(motorSpeed);
    }  
  }
  else if (currentMode == "line") {

    if (obstacleDetected) {
      stop();
    }
    else {
      // Robot movement logic based on sensor value (adjusted for Arduino)
      if (data.line > 1.6 && data.line < 2.1) {
        move_forward(motorSpeed);
      }
      else if (data.line > 1.2 && data.line < 1.6) {
        turn_left(64);
      }
      else if (data.line > 0.33 && data.line < 1.2) {
        turn_left(motorSpeed);
      }
      else if (data.line > 2.1 && data.line < 2.7) {
        turn_right(64);
      }
      else if (data.line > 2.7 && data.line < 2.97) {
        turn_right(motorSpeed);
      }
      else if (data.line < 0.33 || data.line > 2.97) {
        stop(); // No movement if sensor is off track
      }
    }
  }
  else if (currentMode == "auto") {
    if (obstacleDetected) {
      // Enter obstacle avoidance state
      obstacleAvoidanceState = true;
      reverse(motorSpeed);
      delay(500);
      turn_right(motorSpeed);
      delay(500);
    } else if (obstacleAvoidanceState) {
      // Continue navigating away for a moment
      turn_right(motorSpeed);
      delay(500);  // Adjust timing as needed
      obstacleAvoidanceState = false; // Reset state after navigating away
    } else {
      // No obstacle, move forward
      move_forward(motorSpeed);
    }
  }

  delay(100);
}

