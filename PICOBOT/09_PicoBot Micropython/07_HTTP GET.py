# Ref : https://randomnerdtutorials.com/raspberry-pi-pico-w-http-requests-micropython/

import pyRTOS	# Import Library
import network
import requests

def connect_wifi(self):
    ssid = 'favoriot@unifi'
    password = 'fav0r10t2017'
    url = "https://apiv2.favoriot.com/v2/streams?device_developer_id=Pico@fiqxqdin&max=1&order=desc"
    headers = {
        "Content-Type": "application/json",     
        "apikey": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VybmFtZSI6ImZpcXhxZGluIiwicmVhZF93cml0ZSI6dHJ1ZSwiaWF0IjoxNzI3NjYwNDcxfQ.AW0uDZsnFuvYCrMX4u26xEridN00zIjLvGXWm1ynm_s"            
    }
    
    wlan = network.WLAN(network.STA_IF)	# Connect to Wi-Fi
    wlan.active(True)
    wlan.connect(ssid, password)
    
    while not wlan.isconnected():	# Wait for connection
        print("Connecting to WiFi...")
        yield [pyRTOS.timeout(1)]	# Check every 1 second

    print("WiFi connected:", wlan.ifconfig())
    
    HTTP_GET(url,headers)
    yield

def HTTP_GET(URL,HEADERS):
    url = URL
    headers = HEADERS
    
    response = requests.get(url=url, headers=headers)
    response_code = response.status_code
    response_content = response.content
    
    print('Response code: ', response_code)
    print('Response content:', response_content)


# Main
pyRTOS.add_task(pyRTOS.Task(connect_wifi))
pyRTOS.start()
