# REF : https://python-socketio.readthedocs.io/en/latest/client.html

import asyncio      
import socketio 
import requests

# Initializes an asynchronous Socket.IO client
sio = socketio.AsyncClient()

#############################
## Defining Event Handlers ##
#############################
@sio.event      # Connect Event
async def connect():
    print('connection established')

@sio.event      # Emitting Event
async def emit_message():
    await sio.emit('v2/streams',{
        'request': "listen",
        'apikey':"YOUR_APIKEY"
    })

@sio.event      # Disconnect Event
async def disconnect():
    print('disconnected from server')

############################################
## Catch-All Event and Namespace Handlers ## 
############################################
@sio.on('v2/streams')       # Listen To Event/namespace Response
def listen_event(data):
    ##############################
    ## Convert to Line Protocol ##
    ##############################
    measurement = "FAVORIOT"

    for result in data.get("results", []):

        tag = result["device_developer_id"]

        fields = result["data"]
        field_set = ",".join([f"{key}={value}" for key, value in fields.items()])

        timestamp = result["timestamp"] * 1_000_000  # Convert to nanoseconds

        line_protocol = f"{measurement},ID={tag} {field_set} {timestamp}"

        print("Converted line protocol data:", line_protocol)

        post_to_influxdb(line_protocol)

# Function to post data to InfluxDB
def post_to_influxdb(line_protocol):
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

async def main():
    await sio.connect('wss://io.favoriot.com')
    await emit_message()
    await sio.wait()

if __name__ == '__main__':
    asyncio.run(main())
