from flask import Flask
import smtplib

FROM = "kevin@couchjam.com"
TO = "6266026651@txt.att.net"

smtpObj = smtplib.SMTP_SSL("smtp.zoho.com", 465)
smtpObj.login("kevin@couchjam.com", "Db1g91Q98l67")

app = Flask(__name__)

@app.route("/")
def main():
    return "Hello world!"

@app.route("/text")
def text():

    print "Hello"
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
