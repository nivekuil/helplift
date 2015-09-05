from flask import Flask

app = Flask(__name__)

@app.route("/")
def main():
    return "Hello world!"

@app.route("/text")
def text():
    # email 6266026651@txt.att.net

    pass

if __name__ == "__main__":
    app.run()
