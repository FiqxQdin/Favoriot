from flask import Flask, request, jsonify
import requests

app = Flask(__name__)

# Global variable to store the received data
stored_data = {}

@app.route("/", methods=["POST"])
def data():
    # Check if the content type is application/json
    if request.is_json:
        global stored_data
        stored_data = request.get_json()  # Store the received JSON data
        print("Favoriot Data:")
        print(stored_data)

        # Extract the "data" portion for InfluxDB
        favoriot_data = stored_data.get("data", {})
        if not favoriot_data:
            return jsonify({"error": "No valid 'data' field found in JSON"}), 400

        print("Converting to line protocol...")
        measurement = "environment"  # Name your measurement appropriately
        field_list = [f"{key}={value}" for key, value in favoriot_data.items()]
        fields = ",".join(field_list)
        lp = f"{measurement} {fields}"
        print(f"Line Protocol: {lp}")

        print("Sending to InfluxDB...")
        url = "https://us-east-1-1.aws.cloud2.influxdata.com/api/v2/write?bucket=Testing&org=Favoriot"
        headers = {
            "Authorization": "Token mPMgPgfLt0stULInRIyVPt3inWbc44jNfoVRJ02_XSmGwU0ib3HzYlsnG04B7Zy9HDH7f6-s2ke5pLIPMyLgrg==",  # Replace with your token
            "Content-Type": "text/plain"
        }

        response = requests.post(url, headers=headers, data=lp)

        # Log and check the response from InfluxDB
        if response.status_code == 204:
            print("Data successfully written to InfluxDB.")
        else:
            print(f"Failed to write to InfluxDB: {response.status_code}")
            print(f"Response: {response.text}")

        return jsonify({"status": "Data processed", "line_protocol": lp}), 201
    else:
        return jsonify({"error": "Content-Type must be application/json"}), 400

@app.route("/", methods=["GET"])
def get_data():
    # Return the stored JSON data
    if stored_data:
        return jsonify(stored_data), 200
    else:
        return jsonify({"error": "No data found"}), 404
