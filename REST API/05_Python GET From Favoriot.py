# Python GET one-time data then POST to Influxdb

import requests

url = "https://apiv2.favoriot.com/v2/streams?device_developer_id=Hibiscus_Sense_ESP32@fiqxqdin&max=1&order=desc"
headers = headers = {
    "Content-Type": "application/json",     
    "apikey": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VybmFtZSI6ImZpcXhxZGluIiwicmVhZF93cml0ZSI6dHJ1ZSwiaWF0IjoxNzI3NjYwNDcxfQ.AW0uDZsnFuvYCrMX4u26xEridN00zIjLvGXWm1ynm_s"            
}

r = requests.get(url=url, headers=headers)

# Check the response status and content
if r.status_code == 200:
    # Successful request, print the data
    print(r.json())
else:
    # Handle the error
    print(f"Error: {r.status_code}, {r.text}")