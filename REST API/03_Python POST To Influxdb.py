import requests

# Data need to send to Influxdb
data = "python temp1=30,temp2=50"

# Influxdb Headers
headers = {
    "Authorization": "Token YOUR_TOKEN",
    "Content-Type": "text/plain"
}

# Request to POST data to Influxdb
# Change to your bucket and organization
r = requests.post("https://us-east-1-1.aws.cloud2.influxdata.com/api/v2/write?bucket=YOUR_BUCKET&org=YOUR_ORG", headers=headers, data=data)

# Print response from Influxdb
print(r.text)
