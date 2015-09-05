from flask import Flask, request, render_template
from geopy.geocoders import Bing
import smtplib

FROM = "pennapps123@gmail.com"

app = Flask(__name__)
geolocator = Bing("AlqYZzlB6mXiSQCMUK9Wum48tQFs6pN0sG6kD43mmtS61jRz7mg72E5QQg5JBJO-")

@app.route("/")
def main():
    return "Hello World"

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

    # SMS message.
    text = requester_name + " just fell down and was not responsive!" + \
           " This happened at " + location.address + ", coordinates " + \
           latitude + ", " + longitude

    to = phone_number + "@txt.att.net"
    smtpObj = smtplib.SMTP_SSL("smtp.gmail.com", 465)
    smtpObj.login("pennapps123@gmail.com", "XZicRmCQGC0f")

    message = """\
    From: %s
    To: %s
    Subject: text

    %s
    """ % (FROM, ", ".join(to), text)

    print text
    # email 6266026651@txt.att.net
    smtpObj.sendmail(FROM, to, message)

    return("Sending a text to " + to + ", who is at address " + \
           location.address + " at coordinates " + latitude + ", " + longitude)

if __name__ == "__main__":
    app.run(debug=True)
