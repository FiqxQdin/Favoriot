# External module imports
import RPi.GPIO as GPIO
import time
import requests
import json

# Favoriot HTTP API details
url = "YOUR_GATEWAY_URL"  
headers = {
    'Content-Type': 'application/json',
    'apikey': 'YOUR_APIKEY'  # Replace with your actual API key
}

# Pin Definitons:
SENSOR_PIN = 22

# Pin Setup:
GPIO.setmode(GPIO.BCM)              # Use Broadcom pin numbering
GPIO.setup(SENSOR_PIN, GPIO.IN)     # Set GPIO pin SENSOR_PIN as an output

# Read sensor
def read_sensor():
    return "Flame detected" if not GPIO.input(SENSOR_PIN) else "No Flame detected"

# Send data to Favoriot
def send_data_to_favoriot(Flame_status):
    payload = {
        "uid": "Flame",
        "data": {
            "status": Flame_status  # Add Flame sensor status
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
        # Read Flame sensor
        Flame_status = read_sensor()
        print("Flame Sensor Status:", Flame_status)
        
        # Send data to Favoriot
        send_data_to_favoriot(Flame_status)
        
        # Wait for 2 seconds before the next read
        time.sleep(2)

except KeyboardInterrupt:
    print("Program interrupted")
    GPIO.cleanup()  # Clean up the GPIO settings
