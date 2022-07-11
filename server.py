from flask import Flask, request, url_for, redirect, render_template
from flask_mail import Mail
from flask_mail import Message
import serial
import smtplib, ssl
import socket
socket.getaddrinfo('0.0.0.0', 5000)

app = Flask(__name__)
ser = serial.Serial('COM3')

app.config['MAIL_SERVER']='smtp.gmail.com'
app.config['MAIL_PORT'] = 587
app.config['MAIL_USERNAME'] = 'username@gmail.com'
app.config['MAIL_PASSWORD'] = 'password'
app.config['MAIL_USE_TLS'] = True
app.config['MAIL_USE_SSL'] = False
mail = Mail(app)

@app.route("/")
def index():
    serialMessage = ser.readline()
    serialMessage = serialMessage.decode()
    if serialMessage.find("!FLOOD WARNING!") == 0:
        return redirect(url_for("floods"))
    else:
        return render_template("index.html")

@app.route('/message', methods =["GET", "POST"])
def message():
    if request.method == "POST":
        temporary = request.form["message"]
        message = temporary[0: 16]
        serialMessage = "<" + message + ">"
        ser.write(serialMessage.encode())
        returnMessage = "Message Sent to Serial Monitor: " + message
        return redirect(url_for("operation", opr=returnMessage))
    else:
        return render_template("message.html")

@app.route('/led', methods =["GET", "POST"])
def led():
    if request.method == "POST":
        if request.form.get("led_on"):
            returnMessage = "The LED was turned ON"
            serialMessage = 'A'
        elif request.form.get("led_off"):
            returnMessage = "The LED was turned OFF"
            serialMessage = 'S'
        ser.write(serialMessage.encode())
        return redirect(url_for("operation", opr=returnMessage))
    else:
        return render_template("led.html")

@app.route('/rgb', methods =["GET", "POST"])
def rgb():
    if request.method == "POST":
        hex_value = request.form.get("hex_value")
        returnMessage = "The color of the RGB LED was changed to: " + str(hex_value)
        serialMessage = hex_value
        ser.write(serialMessage.encode())
        return redirect(url_for("operation", opr=returnMessage))
    else:
        return render_template("rgb.html")

@app.route('/temperature', methods =["GET", "POST"])
def temperature():
    #Current Temperature: 21 C
    serialTemperature = ser.readline()
    serialTemperature = serialTemperature.decode()
    sensorTemperature = serialTemperature[21:23]
    #sensorTemperature = 19
    if request.method == "POST":
        commandTemperature = request.form.get("commandTemp")
        returnMessage = "Command Temperature sent: " + str(commandTemperature) + " C"
        serialMessage = "@Write Temp " + str(commandTemperature) + " C@"
        ser.write(serialMessage.encode())
        return redirect(url_for("operation", opr=returnMessage))
    else:
        return render_template("temperature.html", sensorTemperature=sensorTemperature)

@app.route('/timetable', methods =["GET", "POST"])
def timetable():
    if request.method == "POST":
        todaysColor = request.form.get("todayColor")
        returnMessage = "Today's color from the selected timetable: " + todaysColor
        serialMessage = todaysColor
        ser.write(serialMessage.encode())
        return redirect(url_for("operation", opr=returnMessage))
    else:
        return render_template("timetable.html")

@app.route('/floods')
def floods():
  msg = Message('!FLOOD WARNING!', sender =   'autumn.dreams1994@gmail.com', recipients = ['bogdan.sfrangeu@gmail.com'])
  msg.body = "Warning, there has been detected a flood in your area!!"
  mail.send(msg)
  return render_template("floods.html")
  
@app.route("/<opr>")
def operation(opr):
    return f"<body style='background-color:#02182B;'><h1 style='text-align:center; font-size:300%; color:#0197F6; position:relative; top:400px;'>{opr}</h1>"
    
if __name__ == '__main__':
    #app.debug = True
    app.run(host="0.0.0.0", port=5000)
