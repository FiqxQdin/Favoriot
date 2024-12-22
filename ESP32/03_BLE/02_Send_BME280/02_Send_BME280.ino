/*
    This code sets up a BLE server on an ESP32 with a single service and characteristic.
    It periodically updates the characteristic with altitude, pressure, humidity, and temperature data
    and sends it to BLE clients every 1 second.
*/

#include <BLEDevice.h>      // BLE device control and initialization
#include <BLEUtils.h>       // Utility functions for BLE operations
#include <BLEServer.h>      // BLE server management
#include <Adafruit_BME280.h> // Library for interacting with the BME280 sensor

// https://www.uuidgenerator.net/version1
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b" // Unique identifier for the BLE service
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8" // Unique identifier for the BLE characteristic

Adafruit_BME280 bme;    // Create a BME280 sensor object for reading environmental data
BLECharacteristic *pCharacteristic;   // Pointer to the BLE characteristic, allowing updates globally

void setup() {
  Serial.begin(115200); // Initialize the serial monitor for debugging
  Serial.println("Starting BLE work!");

  // Initialize the BME280 sensor and check if it is found
  if (!bme.begin()) { 
    // If initialization fails, print an error and halt
    Serial.println("Failed to find Hibiscus Sense BME280 chip");
  }

  // Initialize the BLE device and give it a name
  BLEDevice::init("FIQXQDIN BLE");

  // Create a BLE server to manage BLE client connections
  BLEServer *pServer = BLEDevice::createServer();

  // Create a BLE service within the server
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a characteristic with read and notify properties
  pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID, // Unique identifier for the characteristic
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY // Clients can read and receive updates
  );

  // Initialize the characteristic with a default value
  pCharacteristic->setValue("Initializing...");

  // Start the BLE service to make it available for clients
  pService->start();

  // Configure BLE advertising to make the device discoverable
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID); // Add the service UUID to the advertising packet
  pAdvertising->setScanResponse(true);        // Enable scan response for more advertising data
  pAdvertising->setMinPreferred(0x06);        // Set advertising intervals (min)
  pAdvertising->setMinPreferred(0x12);        // Set advertising intervals (max)

  BLEDevice::startAdvertising();    // Start advertising the BLE service

  Serial.println("BLE Service and Characteristic ready!");
}

void loop() {
  float altitude = bme.readAltitude(1015);    // Read altitude from the BME280 sensor, using the sea-level pressure (in hPa) as a reference
  float pressure = bme.readPressure() / 100.00;   // Read pressure from the sensor and convert it from Pa to Pa
  float humidity = bme.readHumidity();    // Read humidity percentage from the sensor
  float temperature = bme.readTemperature();    // Read temperature in Celsius from the sensor

  // Format the sensor data into a JSON-like string
  char data[128]; // Buffer for the JSON string
  snprintf(data, sizeof(data), 
           "{\"alt\":%.2f,\"press\":%.2f,\"hum\":%.2f,\"temp\":%.2f}", 
           altitude, pressure, humidity, temperature);

  pCharacteristic->setValue(data);    // Update the BLE characteristic with the new data
  pCharacteristic->notify();    // Notify connected BLE clients of the updated data

  Serial.println(data);   // Print the data to the serial monitor for debugging
  delay(1000);    // Delay for 1 second before repeating
}
