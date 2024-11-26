# Python GET data from Favoriot

import requests
import json

##########################
# GET data from Favoriot #
##########################
fav_url = "https://apiv2.favoriot.com/v2/streams"
fav_headers = {
    "Content-Type":"application/json",
    "apikey":"eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VybmFtZSI6ImZpcXhxZGluIiwicmVhZF93cml0ZSI6dHJ1ZSwiaWF0IjoxNzI3NjYwNDcxfQ.AW0uDZsnFuvYCrMX4u26xEridN00zIjLvGXWm1ynm_s"
}
fav_params = {
    "device_developer_id":"Hibiscus_Sense_ESP32@fiqxqdin",
    "max":1,
    "order":"desc"
}
r = requests.get(url=fav_url, headers=fav_headers, params=fav_params)

if r.status_code == 200 :
    json = r.json()
    print("JSON Received:", json)
else :
    print(f"Error: {r.status_code}, {r.text}")

##############################
# Convert into line protocol #
##############################
measurement = "Favoriot"

for result in json["results"]:
    tag = result["device_developer_id"]

    fields = result["data"]
    field_set = ",".join([f"{key}={value}" for key, value in fields.items()])

    timestamp = result["timestamp"] * 1000000

    line_protocol = f"{measurement},ID={tag} {field_set} {timestamp}"

print (line_protocol)

####################
# POST to Influxdb #
####################
InfluxdbURL = "https://us-east-1-1.aws.cloud2.influxdata.com/api/v2/write?bucket=Favoriot&org=Favoriot"
Influxdb_headers = {
    "Authorization": "Token FKguN1aGjZzFRiSGWt632HGoJ9DiJ3hzeTj-qjOppdZhwqZyi81xJm2z-JQtfuBT7SPQf4V9D6EGy-K6ioIEpg==",
    "Content-Type": "text/plain"
}
p = requests.post(url=InfluxdbURL,headers=Influxdb_headers,data=line_protocol)
print(p.status_code)