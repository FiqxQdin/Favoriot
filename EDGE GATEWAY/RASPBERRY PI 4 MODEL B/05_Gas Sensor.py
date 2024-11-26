# External module imports
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

# Pin Definitons:
MQ_PIN = 2

# Pin Setup:
GPIO.setmode(GPIO.BCM)          # Use Broadcom pin numbering
GPIO.setup(MQ_PIN, GPIO.IN)     # Set the MQ sensor digital output as input

# Read sensor
def read_sensor():
    return "Air quality is poor!" if GPIO.input(MQ_PIN) else "Air quality is good!"

# Send data to Favoriot
def send_data_to_favoriot(gas_status):
    payload = {
        "uid": "Gas",
        "data": {
            "status": gas_status  # Add gas sensor status
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
        # Read gas sensor
        gas_status = read_sensor()
        print("Gas Sensor Status:", gas_status)
        
        # Send data to Favoriot
        send_data_to_favoriot(gas_status)
        
        # Wait for 2 seconds before the next read
        time.sleep(2)

except KeyboardInterrupt:
    print("Exiting program")
    GPIO.cleanup()
