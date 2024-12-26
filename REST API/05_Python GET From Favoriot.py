# Python GET one-time data then POST to Influxdb

import requests

url = "https://apiv2.favoriot.com/v2/streams?device_developer_id=YOUR_DEVICE_ID&max=1&order=desc"
headers = {
    "Content-Type": "application/json",     
    "apikey": "YOUR_APIKEY"            
}

r = requests.get(url=url, headers=headers)

# Check the response status and content
if r.status_code == 200:
    # Successful request, print the data
    print(r.json())
else:
    # Handle the error
    print(f"Error: {r.status_code}, {r.text}")
