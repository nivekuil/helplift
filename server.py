from flask import Flask, request, render_template
from geopy.geocoders import Bing
from twilio.rest import TwilioRestClient
from sys import argv

BING_KEY = "AlqYZzlB6mXiSQCMUK9Wum48tQFs6pN0sG6kD43mmtS61jRz7mg72E5QQg5JBJO-"
TWILIO_SID = "AC9867a6902e33cabe8d4085354077882a"
TWILIO_TOKEN = "5298d24f86066d604ce7c690f29b7595"
TWILIO_NUMBER = "+13238922618"

app = Flask(__name__)
geolocator = Bing(BING_KEY)
twilio_client = TwilioRestClient(TWILIO_SID, TWILIO_TOKEN)

@app.route("/")
def main():
    return "Hello world! You shouldn't be seeing this."

@app.route("/config")
def config():
    return render_template("config.html")

@app.route("/text")
def text():

    requester_name = request.args.get('name')
    phone_number = request.args.get('number')
    latitude = request.args.get('lat')
    longitude = request.args.get('lon')

    location = geolocator.reverse((latitude, longitude))

    text = "Watchdog: " + requester_name + \
           " just fell down and was not responsive!" + \
           " This happened at " + location.address + ", coordinates " + \
           latitude + ", " + longitude

    twilio_client.messages.create(to=phone_number, from_=TWILIO_NUMBER,
                                  body=text)

    print text

    return("Sending a text to " + phone_number + ", who is at address " + \
           location.address + " at coordinates " + latitude + ", " + longitude)

if __name__ == "__main__":
    if len(argv) == 2 and argv[1] == 'production':
        app.run(host='0.0.0.0', port=int(port))
    else:
        app.run(debug=True)
