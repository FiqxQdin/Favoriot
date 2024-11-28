#include <NewPing.h>

// Ultrasonic sensor pins
#define TRIGGER_PIN 16
#define ECHO_PIN 17
#define MAX_DISTANCE 200 // Maximum distance to measure (in cm)

// Ultrasonic sensor object
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

void setup() {
  // Serial for debugging
  Serial.begin(115200);
}

void loop() {
  // Read distance from ultrasonic sensor
  unsigned int distance = sonar.ping_cm();

  // Debug output
  if (distance == 0) {
    Serial.println("Object out of range. Max range = 200 cm.");
  } else {
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");
  }

  delay(100); // Allow sensor to stabilize
}