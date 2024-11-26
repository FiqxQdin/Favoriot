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
    url = "https://us-east-1-1.aws.cloud2.influxdata.com/api/v2/write?bucket=Favoriot&org=Favoriot"
    headers = {
        "Authorization": "Token FKguN1aGjZzFRiSGWt632HGoJ9DiJ3hzeTj-qjOppdZhwqZyi81xJm2z-JQtfuBT7SPQf4V9D6EGy-K6ioIEpg==",
        "Content-Type": "text/plain"
    }
    response = requests.post(url, headers=headers, data=lp)

    return jsonify(json), 201

if __name__ == "__main__" :
    app.run(debug=True)