# example to sent light sensor data to favoriot gateway

import RPi.GPIO as GPIO
import time
import requests
import json

# Favoriot HTTP API details
url = "https://apiv2.favoriot.com/v2/gateway/streams/ESP32_GATEWAY@fiqxqdin"  
headers = {
    'Content-Type': 'application/json',
    'apikey': 'eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VybmFtZSI6ImZpcXhxZGluIiwicmVhZF93cml0ZSI6dHJ1ZSwiaWF0IjoxNzI3NjYwNDcxfQ.AW0uDZsnFuvYCrMX4u26xEridN00zIjLvGXWm1ynm_s'  # Replace with your actual API key
}

# Pin Definitions:
SENSOR_PIN = 2

# Pin Setup:
GPIO.setmode(GPIO.BCM)              # Use Broadcom pin numbering
GPIO.setup(SENSOR_PIN, GPIO.IN)     # Set GPIO pin SENSOR_PIN as an input

# Read sensor
def read_light_sensor():
    return "Light detected" if not GPIO.input(SENSOR_PIN) else "No light detected"

# Send data to Favoriot
def send_data_to_favoriot(light_status):
    payload = {
        "uid": "10001",
        "data": {
            "light_status": light_status  # Add light sensor status
        },
        "gateway_name": "Indoor_Gateway"
    }
    try:
        response = requests.post(url, headers=headers, data=json.dumps(payload))
        if response.status_code == 201:
            print("Data sent successfully:", response.json())
        else:
            print("Failed to send data:", response.status_code, response.text)
    except Exception as e:
        print("Error sending data:", e)

try:
    while True:
        # Read light sensor
        light_status = read_light_sensor()
        print("Light Sensor Status:", light_status)
        
        # Send data to Favoriot
        send_data_to_favoriot(light_status)
        
        # Wait for 2 seconds before the next read
        time.sleep(2)

except KeyboardInterrupt:
    print("Program interrupted")
    GPIO.cleanup()  # Clean up the GPIO settings
