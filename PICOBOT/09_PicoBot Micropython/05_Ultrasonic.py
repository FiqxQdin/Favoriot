# Ref : https://docs.sunfounder.com/projects/umsk/en/latest/04_pi_pico/pico_lesson23_ultrasonic.html

from machine import Pin
import time

# Define pin numbers for ultrasonic sensor's TRIG and ECHO pins
TRIG = Pin(16, Pin.OUT)  # TRIG pin set as output
ECHO = Pin(17, Pin.IN)  # ECHO pin set as input

def distance():
    # Function to calculate distance in centimeters
    TRIG.low()  # Set TRIG low
    time.sleep_us(2)  # Wait for 2 microseconds
    TRIG.high()  # Set TRIG high
    time.sleep_us(10)  # Wait for 10 microseconds
    TRIG.low()  # Set TRIG low again

    # Wait for ECHO pin to go high
    while not ECHO.value():
        pass

    time1 = time.ticks_us()  # Record time when ECHO goes high

    # Wait for ECHO pin to go low
    while ECHO.value():
        pass

    time2 = time.ticks_us()  # Record time when ECHO goes low

    # Calculate the duration of the ECHO pin being high
    during = time.ticks_diff(time2, time1)

    # Return the calculated distance (using speed of sound)
    return during * 340 / 2 / 10000  # Distance in centimeters

# Main loop
while True:
    dis = distance()  # Get distance from sensor
    print("Distance: %.2f cm" % dis)  # Print distance
    time.sleep_ms(300)  # Wait for 300 milliseconds before next measurement