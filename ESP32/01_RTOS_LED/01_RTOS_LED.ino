/*Project Title: Multi-Threaded LED Control with FreeRTOS on Hibiscus Sense ESP32.
   
  Description:
  This project demonstrates the use of FreeRTOS on an ESP32 to manage two independent tasks:
  1. Blue LED Control: A task that blinks a single blue LED connected to pin 2 at a specified interval.
  2. RGB LED Control: A task that blinks an RGB LED connected to pin 16 with a purple color (Red=255, Blue=255) at a specified interval.
  
  Each task operates independently on separate cores (if dual-core is available), showcasing basic 
  multi-threading capabilities of the ESP32 with FreeRTOS. The delays for each task are defined 
  individually and passed as parameters during task creation.
  
  Hardware Requirements:
  - Hibiscus Sense ESP32 microcontroller
  - Blue LED connected to pin 2 (the circuit is an active-low circuit)
  - RGB LED connected to pin 16 (using Adafruit NeoPixel-compatible library)
  
  Key Features:
  - Multi-threaded task execution using FreeRTOS
  - Customizable blink intervals for each LED
  - Efficient task management with independent functionality for each LED
  
  Usage:
  - Compile and upload the code to an ESP32 using the Arduino IDE.
  - Observe the independent blinking patterns of the blue LED and the RGB LED.

  Reference:
  Hibiscus Sense ESP32 : https://github.com/myduino/Hibiscus-Sense-Arduino/tree/main
  FreeRTOS Example : https://espressif.github.io/arduino-esp32/package_esp32_index.json

  Description Author: ChatGPT
  Date: 03/12/2024
*/

#if CONFIG_FREERTOS_UNICORE
#define TASK_RUNNING_CORE 0 // Use Core 0 if FreeRTOS is configured for a single core
#else
#define TASK_RUNNING_CORE 1 // Use Core 1 if FreeRTOS is configured for dual-core ESP32
#endif

#include <Adafruit_NeoPixel.h>

// Define the pin for the Blue LED
#define LED_PIN 2

// Initialize the Adafruit NeoPixel object for an RGB LED connected to pin 16
Adafruit_NeoPixel rgb(1, 16); 

// Function prototypes for the tasks
void LED(void *pvParameters); // Task to control the Blue LED
void RGB(void *pvParameters); // Task to control the RGB LED

/**
 * Task to control the Blue LED (connected to LED_PIN).
 * It blinks the LED on and off at an interval specified by the `blink_delay` parameter.
 */
void LED(void *pvParameters) {  
  uint32_t blink_delay = *((uint32_t *)pvParameters); // Retrieve the blink delay passed as a parameter

  // Configure LED_PIN as an output pin
  pinMode(LED_PIN, OUTPUT);

  // Infinite loop to toggle the LED
  for (;;) {                         
    digitalWrite(LED_PIN, HIGH); // Turn the LED ON
    delay(blink_delay);          // Wait for the specified delay
    digitalWrite(LED_PIN, LOW);  // Turn the LED OFF
    delay(blink_delay);          // Wait for the specified delay
  }
}

/**
 * Task to control the RGB LED (connected to pin 16).
 * It blinks the RGB LED with a purple color (Red=255, Blue=255) and turns it off at a specified interval.
 */
void RGB(void *pvParameters) {  
  uint32_t blink_delay = *((uint32_t *)pvParameters); // Retrieve the blink delay passed as a parameter

  // Initialize the NeoPixel object
  rgb.begin(); // Prepare the RGB LED for operation
  rgb.show();  // Turn off all pixels initially

  // Infinite loop to toggle the RGB LED
  for (;;) {                         
    rgb.setPixelColor(0, 255, 0, 255); // Set the first pixel to purple (R=255, G=0, B=255)
    rgb.show();                        // Update the LED to display the color
    delay(blink_delay);                // Wait for the specified delay

    rgb.setPixelColor(0, 0, 0, 0);     // Turn off the LED (set color to black)
    rgb.show();                        // Update the LED to turn it off
    delay(blink_delay);                // Wait for the specified delay
  }
}

/**
 * The setup function initializes the serial communication and creates two tasks:
 * 1. `LED` task to control the Blue LED.
 * 2. `RGB` task to control the RGB LED.
 */
void setup() {
  Serial.begin(115200); // Initialize serial communication for debugging

  // Delay for the Blue LED task (in milliseconds)
  uint32_t task_delay = 1000;  
  xTaskCreate( LED,               // Function to be executed
               "Task Blink 1",    // Task name (for debugging purposes)
               2048,              // Stack size (in bytes)
               (void *)&task_delay, // Pass the delay as a parameter
               2,                 // Task priority
               NULL);             // Task handle (not used)

  // Delay for the RGB LED task (in milliseconds)
  uint32_t blink_delay = 1000;  
  xTaskCreate( RGB,               // Function to be executed
               "Task Blink 2",    // Task name (for debugging purposes)
               2048,              // Stack size (in bytes)
               (void *)&blink_delay, // Pass the delay as a parameter
               2,                 // Task priority
               NULL);             // Task handle (not used)

  Serial.printf("Basic Multi-Threading Arduino Example\n"); // Debug message
}

/**
 * The loop function is intentionally left empty as all functionality is handled by the tasks.
 */
void loop() {
  // Empty loop since tasks handle all operations
}
