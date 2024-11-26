# External module imports
import RPi.GPIO as GPIO
from time import sleep

# Pin Definitons:
LED_PIN = 2

# Pin Setup:
GPIO.setmode(GPIO.BCM)          # Use Broadcom pin numbering
GPIO.setup(LED_PIN, GPIO.OUT)   # Set GPIO pin LED_PIN as an output

try:
    while True:
        GPIO.output(LED_PIN, GPIO.HIGH)    # Turn LED on (set pin to HIGH)
        sleep(1)                           # Wait for 1 second
        GPIO.output(LED_PIN, GPIO.LOW)     # Turn LED off (set pin to LOW)
        sleep(1)                           # Wait for 1 second

except KeyboardInterrupt:
    print("Exiting program")
    GPIO.cleanup()   # Clean up the GPIO settings
