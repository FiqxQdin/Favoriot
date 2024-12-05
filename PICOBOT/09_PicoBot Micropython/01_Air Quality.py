# Ref: https://wiki.seeedstudio.com/Grove-Air_Quality_Sensor_v1.3/

# Import Library
from machine import Pin, ADC
from time import sleep

# Pin Declaration
airQuality_pin = 28

adc = ADC(airQuality_pin)

while True:
  airQuality_raw = adc.read_u16() # read value, 0-65535 across voltage range 0.0v - 3.3v
  airQuality_volt = (airQuality_raw/65535)*3.3
  print("Air Quality Raw : ", airQuality_raw)
  print("Air Quality Voltage : ", airQuality_volt)
  sleep(1)
