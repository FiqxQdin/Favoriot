#include <DHT.h>

#define DHTPIN 5         // Pin connected to the DHT11 sensor (GPIO 15 in this case)
#define DHTTYPE DHT11     // Define the sensor type as DHT11

DHT dht(DHTPIN, DHTTYPE);  // Create an instance of the DHT class

void setup() {
  Serial.begin(115200);      // Start the serial communication at 115200 baud rate
  dht.begin();               // Initialize the DHT sensor
}

void loop() {
  // Reading temperature and humidity values from the DHT11 sensor
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();  // Temperature in Celsius

  // Print the values to the serial monitor
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print("%RH\t");
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println("°C");

  // Wait a few seconds between readings.
  delay(2000);
}
