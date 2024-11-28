const int airQualityPin = 28;  // Analog pin for air quality sensor (GPIO 28)
int airQualityValue = 0;       // Variable to store the sensor's reading

void setup() {
  Serial.begin(115200);        // Start serial communication
}

void loop() {
  airQualityValue = analogRead(airQualityPin);  // Read analog value from the sensor
  float voltage = airQualityValue * (3.3 / 1023);  // Convert to voltage (3.3V reference voltage, 10-bit resolution)

  // Print the value and voltage to the Serial Monitor
  Serial.print("Air Quality Sensor Value: ");
  Serial.print(airQualityValue);
  Serial.print("  Voltage: ");
  Serial.println(voltage, 3);

  delay(1000);  // Wait for 1 second before the next reading
}