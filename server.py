from flask import Flask, request, render_template
import smtplib

FROM = "pennapps123@gmail.com"

app = Flask(__name__)

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

    to = phone_number + "@txt.att.net"
    smtpObj = smtplib.SMTP_SSL("smtp.gmail.com", 465)
    smtpObj.login("pennapps123@gmail.com", "XZicRmCQGC0f")
    # email 6266026651@txt.att.net
    message = """\
    From: %s
    To: %s
    Subject: text

    %s
    """ % (FROM, ", ".join(to),
           requester_name + " just fell and may need your help!"
           + " The distress call came from " + latitude + " " + longitude)

    print "Sending email to", to
    # smtpObj.sendmail(FROM, to, message)

    return("Sending a text..")

if __name__ == "__main__":
    app.run(debug=True)
