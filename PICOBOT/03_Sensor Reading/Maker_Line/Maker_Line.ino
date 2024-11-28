// Sensor pin
const int SA = 26; // Line sensor (analog input)

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
}

void loop() {
  // Read analog value from the line sensor
  int sensorValue = analogRead(SA);
  float voltage = (sensorValue * 3.3) / 1023.0;  // Convert to voltage

  // Print the sensor value (for debugging)
  Serial.println(sensorValue);
  Serial.println(voltage);

  delay(100);  // Small delay for stability
}