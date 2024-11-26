import requests

# Data need to send to Influxdb
data = "python temp1=30,temp2=50"

# Influxdb Headers
headers = {
    "Authorization": "Token FKguN1aGjZzFRiSGWt632HGoJ9DiJ3hzeTj-qjOppdZhwqZyi81xJm2z-JQtfuBT7SPQf4V9D6EGy-K6ioIEpg==",
    "Content-Type": "text/plain"
}

# Request to POST data to Influxdb
r = requests.post("https://us-east-1-1.aws.cloud2.influxdata.com/api/v2/write?bucket=Favoriot&org=Favoriot", headers=headers, data=data)

# Print response from Influxdb
print(r.text)