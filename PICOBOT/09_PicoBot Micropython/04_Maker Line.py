# Ref: https://RandomNerdTutorials.com/raspberry-pi-pico-analog-inputs-micropython/
#      https://docs.micropython.org/en/latest/library/machine.ADC.html

# Import Library
from machine import Pin, ADC
from time import sleep

# Pin Declaration
maker_pin = 26

adc = ADC(maker_pin)

while True:
  maker_raw = adc.read_u16() # read value, 0-65535 across voltage range 0.0v - 3.3v
  maker_volt = (maker_raw/65535)*3.3
  print("Maker Raw : ", maker_raw)
  print("Maker Voltage : ", maker_volt)
  sleep(1)