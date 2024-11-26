# get data streams every 15s
# ctrl + c to stop

import requests
import time

url = "https://apiv2.favoriot.com/v2/streams?device_developer_id=YOUR_DEVICE_ID&max=1&order=desc"
headers = {
    "Content-Type": "application/json",     
    "apikey": "YOUR_APIKEY"            
}

try:
    while True:
        r = requests.get(url=url, headers=headers)
        
        if r.status_code == 200:
            print(r.json())
        else:
            print(f"Error: {r.status_code}, {r.text}")
        
        time.sleep(15)

except KeyboardInterrupt:
    print("Stopped by user.")
