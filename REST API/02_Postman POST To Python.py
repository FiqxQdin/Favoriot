from flask import Flask, request, jsonify

app = Flask(__name__)

@app.route("/data", methods=["POST"])
def data() :
    json = request.get_json()

    return jsonify(json), 201

if __name__ == "__main__" :
    app.run(debug=True)