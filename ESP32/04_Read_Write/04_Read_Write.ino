/*

Code Summary by ChatGPT :
This program establishes a BLE (Bluetooth Low Energy) connection between an ESP32 microcontroller and a smartphone. 
It performs the following:

1. BLE Server Setup:
  The ESP32 acts as a BLE server with two characteristics:
    TX (Notify): Sends sensor data to the smartphone.
    RX (Write): Receives commands from the smartphone.

2. BME280 Sensor Integration:
  The ESP32 reads environmental data (temperature, humidity, pressure, and altitude) from the BME280 sensor.
  This data is sent to the smartphone as JSON via the TX characteristic.

3. LED Control:
  The smartphone can send "ON" or "OFF" commands via the RX characteristic to control an LED connected to GPIO pin 2.

4. BLE Communication:
  The ESP32 advertises the BLE service, allowing the smartphone to connect and interact with it.
  This program demonstrates a combination of BLE communication and sensor data transmission for IoT applications.

*/

#include <BLEDevice.h>      // Library to initialize and control BLE devices.
#include <BLEUtils.h>       // Library for BLE utility functions (e.g., UUID handling).
#include <BLEServer.h>      // Library to manage BLE server operations.
#include <Adafruit_BME280.h> // Library to interact with the BME280 sensor for environmental data readings.

// Define unique identifiers (UUIDs) for the BLE service and characteristics.
// Use a UUID generator to create these.
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b" // UUID for the BLE service.
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8" // UUID for data notifications (TX).
#define CHARACTERISTIC_UUID_RX "f64a319c-b8ec-11ef-9cd2-0242ac120002" // UUID for receiving data (RX).

#define LED_PIN 2  // Define GPIO pin 2 as the pin for controlling the LED.

// Create an instance of the BME280 sensor.
Adafruit_BME280 bme;

// Create a pointer for the BLE characteristic so it can be accessed globally.
BLECharacteristic *pCharacteristic;

// Variables for received BLE messages.
String receivedText = "";           // Stores data received from the BLE client.
unsigned long lastMessageTime = 0;  // Tracks the timestamp of the last received message.

// Custom BLE server callbacks to handle connection and disconnection events.
class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *pServer) {
    // Called when a BLE client connects.
    Serial.println("Connected");  // Log the connection event.
  }

  void onDisconnect(BLEServer *pServer) {
    // Called when a BLE client disconnects.
    Serial.println("Disconnected");  // Log the disconnection event.
  }
};

// Custom BLE characteristic callbacks to handle write operations.
class MyCharacteristicCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    // Called when the client writes data to the RX characteristic.

    // Retrieve the value written by the client.
    std::string value = std::string(pCharacteristic->getValue().c_str());
    receivedText = String(value.c_str());  // Convert the value to a String.

    // Update the last message time.
    lastMessageTime = millis();

    // Print the received data to the serial monitor.
    Serial.print("Received: ");
    Serial.println(receivedText);

    // Control the LED based on the received command.
    if (receivedText == "ON") {
      digitalWrite(LED_PIN, LOW); // Turn on the LED (LOW is ON for active-low LEDs).
    } else if (receivedText == "OFF") {
      digitalWrite(LED_PIN, HIGH); // Turn off the LED.
    }
  }
};

void setup() {
  // Begin serial communication for debugging.
  Serial.begin(115200);
  Serial.println("Starting BLE work!");

  // Configure the LED pin as an output and set it to OFF (HIGH for active-low LEDs).
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  // Initialize the BME280 sensor.
  if (!bme.begin()) {
    // If the sensor is not found, log an error and stop execution.
    Serial.println("Failed to find Hibiscus Sense BME280 chip");
    while (1); // Halt execution.
  }

  // Initialize the BLE device and assign it a name.
  BLEDevice::init("FIQXQDIN BLE");

  // Create a BLE server and set its connection/disconnection callbacks.
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create a BLE service with the defined UUID.
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE characteristic for sending notifications (TX).
  pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ 
  );

  // Create a BLE characteristic for receiving data (RX).
  BLECharacteristic *pCharacteristicRX = pService->createCharacteristic(
    CHARACTERISTIC_UUID_RX,
    BLECharacteristic::PROPERTY_WRITE
  );
  pCharacteristicRX->setCallbacks(new MyCharacteristicCallbacks()); // Set the write callback.

  // Set an initial value for the TX characteristic.
  pCharacteristic->setValue("Initializing...");

  // Start the BLE service.
  pService->start();

  // Configure and start BLE advertising to make the device discoverable.
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID); // Add the service UUID to the advertisement.
  pAdvertising->setScanResponse(true);        // Enable scan responses.
  pAdvertising->setMinPreferred(0x06);        // Set the minimum preferred connection interval.
  pAdvertising->setMinPreferred(0x12);        // Set the maximum preferred connection interval.
  BLEDevice::startAdvertising();              // Start advertising.

  Serial.println("BLE Service and Characteristic ready!");
}

void loop() {
  // Read environmental data from the BME280 sensor.
  float altitude = bme.readAltitude(1015);      // Altitude in meters (sea-level pressure = 1015 hPa).
  float pressure = bme.readPressure() / 100.0;  // Pressure in hPa.
  float humidity = bme.readHumidity();          // Relative humidity in %.
  float temperature = bme.readTemperature();   // Temperature in °C.

  // Format the sensor data into a JSON-like string.
  char data[128]; // Buffer for the formatted string.
  snprintf(data, sizeof(data),
           "{\"alt\":%.2f,\"press\":%.2f,\"hum\":%.2f,\"temp\":%.2f}",
           altitude, pressure, humidity, temperature);

  // Update the TX characteristic with the sensor data and notify the client.
  pCharacteristic->setValue(data);
  pCharacteristic->notify();

  // Print the sensor data to the serial monitor for debugging.
  Serial.println(data);

  // If there is new data from the client, process it.
  if (receivedText.length() > 0 && millis() - lastMessageTime > 1000) {
    // Log the received message.
    Serial.print("Received message: ");
    Serial.println(receivedText);

    // Update the TX characteristic with the received message and notify the client.
    pCharacteristic->setValue(receivedText.c_str());
    //pCharacteristic->notify();

    // Clear the received text to indicate it has been processed.
    receivedText = "";
  }
}
