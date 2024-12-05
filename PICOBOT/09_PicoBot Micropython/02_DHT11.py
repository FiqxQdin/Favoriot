# Ref : https://docs.micropython.org/en/latest/esp8266/tutorial/dht.html

import dht
from machine import Pin
from time import sleep

dht_pin = 5

dht = dht.DHT11(dht_pin)

while True:
    dht.measure()
    temperature = dht.temperature()
    humidity = dht.humidity()
    print ("Temperature : ", temperature, "°C")
    print ("Humidity : ", humidity, "%RH")
    sleep(1)
    