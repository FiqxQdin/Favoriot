import RPi.GPIO as GPIO
import time
import requests
import json

# Favoriot HTTP API details
url = "YOUR_URL"  
headers = {
    'Content-Type': 'application/json',
    'apikey': 'YOUR_APIKEY' 
}

# Pin Definitions:
SENSOR_PIN = 2

# Pin Setup:
GPIO.setmode(GPIO.BCM)              # Use Broadcom pin numbering
GPIO.setup(SENSOR_PIN, GPIO.IN)     # Set GPIO pin SENSOR_PIN as an input

# Read sensor
def read_sensor():
    return "Light detected" if not GPIO.input(SENSOR_PIN) else "No light detected"

# Send data to Favoriot
def send_data_to_favoriot(light_status):
    payload = {
        "uid": "Light",
        "data": {
            "status": light_status  # Add light sensor status
        }
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
        light_status = read_sensor()
        print("Light Sensor Status:", light_status)
        
        # Send data to Favoriot
        send_data_to_favoriot(light_status)
        
        # Wait for 2 seconds before the next read
        time.sleep(2)

except KeyboardInterrupt:
    print("Program interrupted")
    GPIO.cleanup()  # Clean up the GPIO settings
