from gpiozero import LED
from time import sleep

led = LED(2)    # Define LED on GPIO pin 2

try:
    while True:
        led.on()    # Turn LED on
        sleep(1)    # Wait for 1 second
        led.off()   # Turn LED off
        sleep(1)    # Wait for 1 second

except KeyboardInterrupt:
    print("Exiting program")
    led.off()   # Ensure the LED is turned off when exiting