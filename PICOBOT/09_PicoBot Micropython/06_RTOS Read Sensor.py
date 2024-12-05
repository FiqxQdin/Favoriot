# Ref : https://my.cytron.io/tutorial/real-time-multitasking-on-maker-pi-pico-using-pyrtos

import pyRTOS
import dht
import time
from machine import Pin, ADC

def read_makerLine(self):
    maker_pin = 26
    adc = ADC(maker_pin)
    yield

    while True:
        maker_raw = adc.read_u16() # read value, 0-65535 across voltage range 0.0v - 3.3v
        maker_volt = (maker_raw/65535)*3.3
        print("Maker Raw : ", maker_raw)
        print("Maker Voltage : ", maker_volt)
        yield [pyRTOS.timeout(1)]           # Delay in seconds (Other task can run)

def read_lightSensor(self):
    ldr_pin = 27
    adc = ADC(ldr_pin)
    yield

    while True:
        ldr_raw = adc.read_u16() # read value, 0-65535 across voltage range 0.0v - 3.3v
        ldr_volt = (ldr_raw/65535)*3.3
        print("LDR Raw : ", ldr_raw)
        print("LDR Voltage : ", ldr_volt)
        yield [pyRTOS.timeout(1)]           # Delay in seconds (Other task can run)

def read_DHT11(self):
    dht_pin = 5
    dht11 = dht.DHT11(dht_pin)
    yield

    while True:
        dht11.measure()
        temperature = dht11.temperature()
        humidity = dht11.humidity()
        print ("Temperature : ", temperature, "°C")
        print ("Humidity : ", humidity, "%RH")
        yield [pyRTOS.timeout(1)]           # Delay in seconds (Other task can run)

def read_airQuality(self):
    airQuality_pin = 28
    adc = ADC(airQuality_pin)
    yield

    while True:
        airQuality_raw = adc.read_u16() # read value, 0-65535 across voltage range 0.0v - 3.3v
        airQuality_volt = (airQuality_raw/65535)*3.3
        print("Air Quality Raw : ", airQuality_raw)
        print("Air Quality Voltage : ", airQuality_volt)
        yield [pyRTOS.timeout(1)]           # Delay in seconds (Other task can run)

def read_ultrasonic(self):
    TRIG = Pin(16, Pin.OUT)  # TRIG pin set as output
    ECHO = Pin(17, Pin.IN)  # ECHO pin set as input
    yield

    while True:
        dis = distance(TRIG, ECHO)  # Get distance from sensor
        print("Distance: %.2f cm" % dis)  # Print distance
        pyRTOS.timeout(0.3)  # Wait for 300 milliseconds before next measurement
        yield [pyRTOS.timeout(1)]           # Delay in seconds (Other task can run)

def distance(TRIG, ECHO):
    # Function to calculate distance in centimeters
    TRIG.low()  # Set TRIG low
    pyRTOS.timeout(0.000002)  # Wait for 2 microseconds
    TRIG.high()  # Set TRIG high
    pyRTOS.timeout(0.00001)  # Wait for 10 microseconds
    TRIG.low()  # Set TRIG low again
    # Wait for ECHO pin to go high
    while not ECHO.value():
        pass
    # Record time when ECHO goes high
    time1 = time.ticks_us() 
    # Wait for ECHO pin to go low
    while ECHO.value():
        pass
    # Record time when ECHO goes low
    time2 = time.ticks_us()
    # Calculate the duration of the ECHO pin being high
    during = time.ticks_diff(time2, time1)
    # Return the calculated distance (using speed of sound)
    return during * 340 / 2 / 10000  # Distance in centimeters



# Main
pyRTOS.add_task(pyRTOS.Task(read_makerLine))          # Add Task
pyRTOS.add_task(pyRTOS.Task(read_lightSensor))
pyRTOS.add_task(pyRTOS.Task(read_DHT11))
pyRTOS.add_task(pyRTOS.Task(read_airQuality))
pyRTOS.add_task(pyRTOS.Task(read_ultrasonic))
pyRTOS.start()                              # Start pyRTOS
