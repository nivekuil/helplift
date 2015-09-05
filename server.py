from flask import Flask
import smtplib

FROM = "pennapps123@gmail.com"
TO = "6266026651@txt.att.net"
TEXT = "Hello"


app = Flask(__name__)

@app.route("/")
def main():
    return "Hello world!"

@app.route("/text")
def text():

    print "Hello"
    smtpObj = smtplib.SMTP_SSL("smtp.gmail.com", 465)
    smtpObj.login("pennapps123@gmail.com", "XZicRmCQGC0f")
    # email 6266026651@txt.att.net
    message = """\
    From: %s
    To: %s
    Subject: text

    %s
    """ % (FROM, ", ".join(TO), TEXT)
    smtpObj.sendmail(FROM, TO, message)

    return("Sending a text..")

if __name__ == "__main__":
    app.run(debug=True)
