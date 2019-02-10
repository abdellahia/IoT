# coding: utf-8
from flask import Flask
from coapthon.client.helperclient import HelperClient

app = Flask(__name__)

hostArduino = "192.168.1.245"
port = 5683

@app.route("/get_values")
def get_values(path):
        client = HelperClient(server=(hostArduino, port))
        response = client.get(path)
        client.stop()
        return response._payload

@app.route("/")
def main():
	return "luminosité : " + get_values("luminosite") + "</br>température : " + get_values("temperature") + "°C" + "</br>humidité : " + get_values("humidite") + "%"

if __name__ == "__main__":
  app.run(host="192.168.70.2", debug=True)
