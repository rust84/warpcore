import serial
import os

def serialcmdw():
    os.system('clear')
    serialcmd = input("Enter command:")
    ser.write(serialcmd.encode())

ser = serial.Serial()
os.system('clear')
ser.port = "COM5"
ser.baudrate = 9600
ser.open()

print("Example usage: <2, 160, 220, 255, 1>")
print("Parameters: <Warp Factor, Hue, Saturation, Brightness, Pattern>")

while True:
  serialcmdw()
