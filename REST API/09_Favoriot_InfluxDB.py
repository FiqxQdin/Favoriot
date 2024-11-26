# Python GET data from Favoriot every 15s

import requests
import json
import time

try:
    while True:
        ##########################
        # GET data from Favoriot #
        ##########################
        fav_url = "https://apiv2.favoriot.com/v2/streams"
        fav_headers = {
            "content-type":"application/json",
            "apikey":"YOUR_APIKEY"
        }
        fav_params = {
            "device_developer_id":"YOUR_ID",
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
        InfluxdbURL = "https://us-east-1-1.aws.cloud2.influxdata.com/api/v2/write"
        Influxdb_headers = {
            "Authorization": "Token YOUR_TOKEN",
            "Content-Type": "text/plain"
        }
        Influxdb_params = {
            "bucket":"YOUR_BUCKET",
            "org":"YOUR_ORGANIZATION"
        }
        p = requests.post(url=InfluxdbURL,headers=Influxdb_headers,params=Influxdb_params,data=line_protocol)
        print(p.status_code)

        time.sleep(15)
except KeyboardInterrupt:
    print("Stopped by user.")
