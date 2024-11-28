#include <NewPing.h>

// Ultrasonic sensor pins
#define TRIGGER_PIN 16
#define ECHO_PIN 17
#define MAX_DISTANCE 200 // Maximum distance to measure (in cm)

// Define motor control pins
#define PWM_M1A 8  // Left Motor 1 Forward
#define PWM_M1B 9  // Left Motor 1 Backward
#define PWM_M2A 10 // Right Motor 2 Forward
#define PWM_M2B 11 // Right Motor 2 Backward

// Ultrasonic sensor object
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

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
  // Serial for debugging
  Serial.begin(115200);

  // Set motor control pins as output
  pinMode(PWM_M1A, OUTPUT);
  pinMode(PWM_M1B, OUTPUT);
  pinMode(PWM_M2A, OUTPUT);
  pinMode(PWM_M2B, OUTPUT);
}

void loop() {
  // Read distance from ultrasonic sensor
  unsigned int distance = sonar.ping_cm();

  // Debug output
  if (distance == 0) {
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");
    Serial.println("Object out of range. Max range = 200 cm.");
    move_forward(128);
  } 
  else if (distance > 0 && distance < 10) {
    stop();
    delay(500);
    turn_right(64);
    delay(1200);
  } 
  else {
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");
    move_forward(128);
  }

  delay(100); // Allow sensor to stabilize
}