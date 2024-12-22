/*
  Code Description:

  This code demonstrates how to create a simple Bluetooth Low Energy (BLE) server using an ESP32. 
  The BLE server allows a connected client (e.g., a smartphone app) to exchange data with the ESP32 through two characteristics:
    1. RX Characteristic (Receive):  
      This characteristic allows the BLE client to send data (write requests) to the ESP32. 
      The received data is processed and logged to the serial monitor.
    2. TX Characteristic (Transmit):  
      This characteristic allows the ESP32 to send notifications to the BLE client. 
      Notifications are triggered either when the ESP32 receives a message from the BLE client or 
      when the user inputs data through the serial monitor.

  Key Features:
    - BLE Initialization:  
      The ESP32 is configured as a BLE server with a custom service and two characteristics (TX and RX), each identified by unique UUIDs.
    - Event Handling with Callbacks:  
      - Connection and disconnection events are handled to log the status of the BLE server.
      - Writing data to the RX characteristic triggers a callback to process and log the received data.
    - Serial Input Integration:  
      The ESP32 can read user input from the serial monitor and send it as a BLE notification to the client.
    - Throttled Notifications:  
      The code ensures that notifications are sent no more than once per second to avoid overwhelming the BLE client.

  Use Case:
    This program is suitable for applications where a BLE client (like a mobile app or another BLE-enabled device) needs to:
    - Send commands or data to the ESP32 (e.g., controlling a device or logging information).
    - Receive updates or notifications from the ESP32 (e.g., sensor data or status updates).  

    The setup makes it ideal for IoT projects, remote control systems, or data logging solutions.

  Reference: https://docs.sunfounder.com/projects/esp32-starter-kit/en/latest/arduino/basic_projects/ar_bluetooth.html
*/

// These headers include the necessary libraries to work with Bluetooth Low Energy (BLE) on the ESP32.
#include "BLEDevice.h"    // BLEDevice provides the main interface for initializing BLE and creating servers.
#include "BLEServer.h"    // BLEServer is used to create a BLE server.
#include "BLEUtils.h"     // BLEUtils provides additional utilities for working with BLE.
#include "BLE2902.h"      // BLE2902 is used to add a Client Characteristic Configuration Descriptor (CCCD), which allows notifications.

// This defines the name of the BLE device that will be displayed when scanning for devices. 
// The name helps clients identify this specific device when it advertises itself.
const char *bleName = "ESP32_Bluetooth";

// `receivedText` is used to store the latest text message received from a BLE client.
// `lastMessageTime` records the timestamp (in milliseconds) of when the last message was received. 
// This ensures notifications are sent no more than once per second.
String receivedText = "";
unsigned long lastMessageTime = 0;

// https://www.uuidgenerator.net/version1
// These are UUIDs (universally unique identifiers) for the BLE service and its characteristics.
#define SERVICE_UUID           "e7f85fe2-b8ec-11ef-9cd2-0242ac120002"   // SERVICE_UUID identifies the custom service provided by this BLE device.
#define CHARACTERISTIC_UUID_RX "edcb6be4-b8ec-11ef-9cd2-0242ac120002"   // CHARACTERISTIC_UUID_RX is the characteristic for receiving data (written by the client).
#define CHARACTERISTIC_UUID_TX "f64a319c-b8ec-11ef-9cd2-0242ac120002"   // CHARACTERISTIC_UUID_TX is the characteristic for sending data (notifications from the server).

// This pointer is used to reference the characteristic that sends notifications to the client.
BLECharacteristic *pCharacteristic;

void setup() {
  Serial.begin(115200);  // Initializes the serial communication at a baud rate of 115200 for debugging.
  setupBLE();            // Calls the function to set up the BLE server, services, and characteristics.
}

void loop() {
  // Check if there is new data in `receivedText` and whether 1 second has passed since the last notification.
  if (receivedText.length() > 0 && millis() - lastMessageTime > 1000) {
    Serial.print("Received message: ");
    Serial.println(receivedText);  // Prints the received message to the serial monitor.
    pCharacteristic->setValue(receivedText.c_str());  // Updates the TX characteristic with the new message.
    pCharacteristic->notify();  // Notifies the connected BLE client with the updated characteristic value.
    receivedText = "";  // Clears `receivedText` to indicate the message has been processed.
  }

  // Check if there's data available on the serial port (from the user via serial input).
  if (Serial.available() > 0) {
    String str = Serial.readStringUntil('\n');  // Reads a line of input until a newline character.
    const char *newValue = str.c_str();  // Converts the string to a C-style string (const char *).
    pCharacteristic->setValue(newValue);  // Updates the TX characteristic with this new value.
    pCharacteristic->notify();  // Sends a notification with the new value to the connected BLE client.
  }
}

class MyServerCallbacks : public BLEServerCallbacks {
  // This class provides custom behavior for BLE server events such as connection and disconnection.

  void onConnect(BLEServer *pServer) {
    // This function is called when a client connects to the BLE server.
    Serial.println("Connected");  // Prints a message to indicate a successful connection.
  }

  void onDisconnect(BLEServer *pServer) {
    // This function is called when a client disconnects from the BLE server.
    Serial.println("Disconnected");  // Prints a message to indicate the disconnection.
  }
};

class MyCharacteristicCallbacks : public BLECharacteristicCallbacks {
  // This class provides custom behavior for BLE characteristic events such as writing data.

  void onWrite(BLECharacteristic *pCharacteristic) {
    // This function is called when a client writes data to the RX characteristic.
    std::string value = std::string(pCharacteristic->getValue().c_str());  // Gets the written value as a C++ string.
    receivedText = String(value.c_str());  // Converts the value to an Arduino String and stores it in `receivedText`.
    lastMessageTime = millis();  // Updates the timestamp for when the message was received.
    Serial.print("Received: ");  // Prints the received message to the serial monitor.
    Serial.println(receivedText);
  }
};

void setupBLE() {
  BLEDevice::init(bleName);  // Initializes the BLE device with the defined name.
  BLEServer *pServer = BLEDevice::createServer();  // Creates a BLE server instance.

  if (pServer == nullptr) {
    // Prints an error if the BLE server fails to initialize.
    Serial.println("Error creating BLE server");
    return;
  }

  pServer->setCallbacks(new MyServerCallbacks());  // Sets the server's connection and disconnection callbacks.

  BLEService *pService = pServer->createService(SERVICE_UUID);  // Creates a custom BLE service.

  if (pService == nullptr) {
    // Prints an error if the BLE service fails to initialize.
    Serial.println("Error creating BLE service");
    return;
  }

  // Creates a characteristic for sending notifications (TX characteristic).
  pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID_TX, BLECharacteristic::PROPERTY_NOTIFY);
  pCharacteristic->addDescriptor(new BLE2902());  // Adds a CCCD descriptor to enable notifications.

  // Creates a characteristic for receiving data (RX characteristic).
  BLECharacteristic *pCharacteristicRX = pService->createCharacteristic(
    CHARACTERISTIC_UUID_RX, BLECharacteristic::PROPERTY_WRITE);
  pCharacteristicRX->setCallbacks(new MyCharacteristicCallbacks());  // Sets callbacks for when data is written.

  pService->start();  // Starts the BLE service to make it available for clients.
  pServer->getAdvertising()->start();  // Starts advertising the BLE server so clients can discover it.

  Serial.println("Waiting for a client connection...");  // Prints a message to indicate readiness.
}
