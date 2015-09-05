from flask import Flask
from flask_mail import Message, Mail

app = Flask(__name__)
app.config["MAIL_SERVER"] = "smtp.zoho.com"
app.config["MAIL_PORT"] = 465
app.config["MAIL_USE_SSL"] = True
app.config["MAIL_USERNAME"] = "kevin@couchjam.com"
app.config["MAIL_PASSWORD"] = "Db1g91Q98l67"
mail = Mail(app)

@app.route("/")
def main():
    return "Hello world!"

@app.route("/text")
def text():

    # email 6266026651@txt.att.net
    msg = Message("Hello", sender="nivekuil@gmail.com",
                  recipients="6266026651@txt.att.net")

    mail.send(msg)
    return("Sending a text..")

if __name__ == "__main__":
    app.run(debug=True)
