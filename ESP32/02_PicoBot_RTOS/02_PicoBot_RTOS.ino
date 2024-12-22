#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <NewPing.h>

// Ultrasonic sensor pins
#define TRIGGER_PIN 16
#define ECHO_PIN 17
#define MAX_DISTANCE 200 // Maximum distance to measure (in cm)

// Ultrasonic sensor object
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

// Semaphore handle to manage access to the serial port
SemaphoreHandle_t xSerialSemaphore;

void readUltrasonicSensor(void *pvParameters) {
  (void) pvParameters;
  unsigned int distance;
  while (1) {
    // Read distance from ultrasonic sensor
    distance = sonar.ping_cm();
    
    // Take the semaphore to ensure exclusive access to the serial port
    if (xSemaphoreTake(xSerialSemaphore, portMAX_DELAY) == pdTRUE) {
      if (distance == 0) {
        Serial.println("Object out of range. Max range = 200 cm.");
      } else {
        Serial.print("Distance: ");
        Serial.print(distance);
        Serial.println(" cm");
      }
      xSemaphoreGive(xSerialSemaphore); // Release the semaphore
    }
    
    // Wait for a short period before the next reading (100ms)
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  
  // Create a binary semaphore for managing serial access
  xSerialSemaphore = xSemaphoreCreateBinary();
  xSemaphoreGive(xSerialSemaphore); // Initially give the semaphore

  // Create the task that reads the ultrasonic sensor
  xTaskCreate(readUltrasonicSensor, "Read Ultrasonic", 256, NULL, 1, NULL);

  // Start the FreeRTOS scheduler
  vTaskStartScheduler();
}

void loop() {
  // In FreeRTOS, the loop function is not used, as tasks are handled by the scheduler.
  // The scheduler will handle the execution of the readUltrasonicSensor task.
}
