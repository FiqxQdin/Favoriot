// Define motor control pins
#define PWM_M1A 8  // Left Motor 1 Forward
#define PWM_M1B 9  // Left Motor 1 Backward
#define PWM_M2A 10 // Right Motor 2 Forward
#define PWM_M2B 11 // Right Motor 2 Backward

const int Sensor_pin = 26; // Line sensor pin = 26

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
  // Initialize serial communication
  Serial.begin(115200);

  // Set motor control pins as output
  pinMode(PWM_M1A, OUTPUT);
  pinMode(PWM_M1B, OUTPUT);
  pinMode(PWM_M2A, OUTPUT);
  pinMode(PWM_M2B, OUTPUT);
}

void loop() {
  // Read analog value from the line sensor
  int sensorValue = analogRead(Sensor_pin);
  float voltage = (sensorValue * 3.3) / 1023.0;  // Convert to voltage

  // Print the sensor value (for debugging)
  Serial.print("Raw Value = ");
  Serial.print(sensorValue);
  Serial.print("\tVoltage Value = ");
  Serial.println(voltage);

  // Robot movement logic based on sensor value (adjusted for Arduino)
  if (voltage > 1.6 && voltage < 2.1) {
    move_forward(128);
  }
  else if (voltage > 1.2 && voltage < 1.6) {
    turn_left(64);
  }
  else if (voltage > 0.33 && voltage < 1.2) {
    turn_left(128);
  }
  else if (voltage > 2.1 && voltage < 2.7) {
    turn_right(64);
  }
  else if (voltage > 2.7 && voltage < 2.97) {
    turn_right(128);
  }
  else if (voltage < 0.33 || voltage > 2.97) {
    stop(); // No movement if sensor is off track
  }

  delay(100);  // Small delay for stability
}