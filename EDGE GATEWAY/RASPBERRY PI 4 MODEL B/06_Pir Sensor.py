# External module imports
import RPi.GPIO as GPIO
import time

# Pin Definitons:
SENSOR_PIN = 2

# Pin Setup:
GPIO.setmode(GPIO.BCM)              # Use Broadcom pin numbering
GPIO.setup(SENSOR_PIN, GPIO.IN)     # Set GPIO pin SENSOR_PIN as an output

# Read sensor
def read_pir_sensor():
    if GPIO.input(SENSOR_PIN):
        print("Motion detected")        # 1 = Motion Detected
    else:
        print("No motion detected")     # 0 = No Motion Detected

try:
    while True:
        read_pir_sensor()
        time.sleep(1)

except KeyboardInterrupt:
    print("Program interrupted")
    GPIO.cleanup()  # Clean up the GPIO settings