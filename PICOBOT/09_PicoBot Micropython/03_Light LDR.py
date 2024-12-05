# Ref: https://RandomNerdTutorials.com/raspberry-pi-pico-analog-inputs-micropython/
#      https://docs.micropython.org/en/latest/library/machine.ADC.html

# Import Library
from machine import Pin, ADC
from time import sleep

# Pin Declaration
ldr_pin = 27

adc = ADC(ldr_pin)

while True:
  ldr_raw = adc.read_u16() # read value, 0-65535 across voltage range 0.0v - 3.3v
  ldr_volt = (ldr_raw/65535)*3.3
  print("LDR Raw : ", ldr_raw)
  print("LDR Voltage : ", ldr_volt)
  sleep(1)
