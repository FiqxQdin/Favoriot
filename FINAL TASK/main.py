"""
REF :
1. Flask Quick Start : https://www.geeksforgeeks.org/flask-tutorial/#flask-quick-start
2. HTML Forms : https://www.w3schools.com/html/html_forms.asp
3. stackoverflow : https://stackoverflow.com/questions/22195065/how-to-send-a-json-object-using-html-form-data

Display graph using http instead of websocket.
PNG Output :
1. https://github.com/FiqxQdin/Favoriot/blob/main/FINAL%20TASK/Pictures/Screenshot%202024-12-19%20110559.png
2. https://github.com/FiqxQdin/Favoriot/blob/main/FINAL%20TASK/Pictures/Screenshot%202024-12-19%20110609.png
3. https://github.com/FiqxQdin/Favoriot/blob/main/FINAL%20TASK/Pictures/Screenshot%202024-12-19%20110930.png
"""

import requests
from flask import Flask, request, render_template, jsonify
from datetime import datetime

fav_data = None

# instance of flask application
app = Flask(__name__)

# Home page that will ask for device_id, apikey, date then POST that to /input
@app.route("/", methods=["GET"])
def home():
    return render_template("home.html")


@app.route("/input", methods=["GET","POST"])
def input():
    global fav_data

    if request.method == "POST" :
        data = request.json

        device_id = data.get('device_id')
        apikey = data.get('apikey')
        date = data.get('date')

        url = "https://apiv2.favoriot.com/v2/streams"
        headers = {
            "Content-Type": "application/json",     
            "apikey": apikey
        }
        parse_date = datetime.strptime(date, "%Y-%m-%dT%H:%M")
        new_date = parse_date.strftime("%Y-%m-%dT%H:%M:%S.%f")[:-3] + "Z"
        parameters = {
            "device_developer_id" : device_id,
            "created_from_to" : f"[{new_date} TO NOW]"
        }
        r = requests.get(url=url, headers=headers, params=parameters)

        # Check the response status and content
        if r.status_code == 200:
            # Successful request, print the data
            fav_data = r.json()

            # Simplify fav_data to only include 'data' and 'timestamp' fields
            if 'results' in fav_data:
                fav_data = {
                    "numFound": fav_data.get("numFound", 0),
                    "results": [
                        {"data": result["data"], "timestamp": result["timestamp"]}
                        for result in fav_data["results"]
                    ],
                }     

            print(fav_data)
        else:
            # Handle the error
            return jsonify({"error": f"Error: {r.status_code}, {r.text}"}), r.status_code

        # Example processing
        response_message = {
            "message": "Data received successfully",
            "device_id": device_id,
            "apikey": apikey,
            "date": date
        }

        return jsonify(response_message), 200
    
    if fav_data :
        # return jsonify(fav_data), 200
        return render_template("input.html", fav_data=fav_data)
    else :
        return jsonify({"error": "No data available. Please POST first."}), 404



if __name__ == '__main__':  
   app.run(debug=True)
