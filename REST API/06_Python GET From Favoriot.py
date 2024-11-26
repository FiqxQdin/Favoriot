# get data streams every 15s
# ctrl + c to stop

import requests
import time

url = "https://apiv2.favoriot.com/v2/streams?device_developer_id=Hibiscus_Sense_ESP32@fiqxqdin&max=1&order=desc"
headers = {
    "Content-Type": "application/json",     
    "apikey": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VybmFtZSI6ImZpcXhxZGluIiwicmVhZF93cml0ZSI6dHJ1ZSwiaWF0IjoxNzI3NjYwNDcxfQ.AW0uDZsnFuvYCrMX4u26xEridN00zIjLvGXWm1ynm_s"            
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