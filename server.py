from flask import Flask
from flask_mail import Message, Mail

app = Flask(__name__)
mail = Mail()

@app.route("/")
def main():
    return "Hello world!"

@app.route("/text")
def text():

    # email 6266026651@txt.att.net
    msg = Message("Hello", sender="nivekuil@gmail.com",
                  recipients="6266026651@txt.att.net")

    mail.send()

if __name__ == "__main__":
    app.run()
