# POST Method
# Postman POST To Python
# Python POST To Influxdb

from flask import Flask, request, jsonify # To let Postman request to POST
import requests                           # To request to POST to Influxdb
import json

app = Flask(__name__)

@app.route("/data", methods=["POST"])
def data() :
    json = request.get_json()
    print("POSTMAN JSON Data: ")
    print(json)

    print("Converting to line protocol...")
    measurement = "python"
    print("measurement: %s" %measurement)
    field_list = [f"{key}={value}" for key, value in json.items()]
    print (field_list)
    fields = ",".join(field_list)
    print(fields)
    lp = f"{measurement} {fields}"
    print(lp)

    print ("send to Influxdb")
    # Change to your bucket and org
    url = "https://us-east-1-1.aws.cloud2.influxdata.com/api/v2/write?bucket=YOUR_BUCKET&org=YOUR_ORGANIZATION"
    headers = {
        "Authorization": "Token YOUR_TOKEN",
        "Content-Type": "text/plain"
    }
    response = requests.post(url, headers=headers, data=lp)

    return jsonify(json), 201

if __name__ == "__main__" :
    app.run(debug=True)
