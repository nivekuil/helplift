from flask import Flask, request, render_template
import smtplib

FROM = "pennapps123@gmail.com"
TEXT = "Someone just fell and needs your help!"

app = Flask(__name__)

@app.route("/")
def main():
    return "Hello World"

@app.route("/config")
def config():
    return render_template("config.html")

@app.route("/text")
def text():

    phone_number = request.args.get('number')
    to = phone_number + "@txt.att.net"
    smtpObj = smtplib.SMTP_SSL("smtp.gmail.com", 465)
    smtpObj.login("pennapps123@gmail.com", "XZicRmCQGC0f")
    # email 6266026651@txt.att.net
    message = """\
    From: %s
    To: %s
    Subject: text

    %s
    """ % (FROM, ", ".join(to), TEXT)
    print "Sending email to", to
    # smtpObj.sendmail(FROM, TO, message)

    return("Sending a text..")

if __name__ == "__main__":
    app.run(debug=True)
