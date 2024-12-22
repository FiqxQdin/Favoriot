/*
    This code sets up a BLE server on an ESP32 with a single service and characteristic.
    The characteristic is read-only, meaning BLE clients can only read the value and not modify it.

    Features:
    - Creates a BLE server with a unique service UUID.
    - Defines a characteristic with a unique UUID and makes it read-only.
    - Advertises the service and allows BLE clients to connect and read the characteristic.

    Useful for applications where the ESP32 needs to share information (e.g., sensor data) in a read-only manner.
*/

#include <BLEDevice.h>    // BLE device library for ESP32
#include <BLEUtils.h>     // Utility functions for BLE
#include <BLEServer.h>    // BLE server functionality

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

// UUIDs for the BLE service and characteristic
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b" // Unique identifier for the BLE service
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8" // Unique identifier for the BLE characteristic

void setup() {
  // Initialize the serial monitor for debugging
  Serial.begin(115200);
  Serial.println("Starting BLE work!");

  // Initialize the BLE device and set the name that will appear in BLE scans
  BLEDevice::init("FIQXQDIN BLE");

  // Create a BLE server
  BLEServer *pServer = BLEDevice::createServer();

  // Create a BLE service with the specified UUID
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE characteristic within the service
  // This characteristic is read-only (PROPERTY_READ)
  BLECharacteristic *pCharacteristic = 
    pService->createCharacteristic(
      CHARACTERISTIC_UUID,                   // Unique identifier for the characteristic
      BLECharacteristic::PROPERTY_READ       // Make the characteristic read-only
    );

  // Set an initial value for the characteristic
  pCharacteristic->setValue("READ ONLY");

  // Start the service so it becomes active and discoverable by clients
  pService->start();

  // Set up advertising so BLE clients can discover the ESP32
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);   // Advertise the service UUID
  pAdvertising->setScanResponse(true);         // Include additional data in advertising packets
  pAdvertising->setMinPreferred(0x06);         // Helps with iPhone connection issues
  pAdvertising->setMinPreferred(0x12);         // Additional connection optimization

  // Start advertising to make the ESP32 visible to BLE clients
  BLEDevice::startAdvertising();
  Serial.println("Characteristic defined! Now you can read it in your phone!");
}

void loop() {
  // Main loop does nothing as BLE functionality runs in the background
  delay(2000);  // Add a delay to reduce power consumption
}
