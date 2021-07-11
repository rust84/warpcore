import serial
import time
import psutil

def serialcmd(warpfactor, hue):
    serialcmd = "<%s, %s, 220, 255, 1>" % (warpfactor, hue)
    print(serialcmd)
    ser.write(serialcmd.encode())

ser = serial.Serial()
ser.port = "COM5"
ser.baudrate = 9600
ser.open()

while True:
  cpupercent = psutil.cpu_percent()
  hue = 160
  warpfactor = 1

  if cpupercent > 10:
    warpfactor = 2
  if cpupercent > 20:
    warpfactor = 3
  if cpupercent > 30:
    warpfactor = 3
  if cpupercent > 40:
    warpfactor = 4
  if cpupercent > 50:
    warpfactor = 5
    hue = 240
  if cpupercent > 60:
    warpfactor = 6
    hue = 240
  if cpupercent > 70:
    warpfactor = 7
    hue = 240
  if cpupercent > 80:
    warpfactor = 8
    hue = 255
  if cpupercent > 90:
    warpfactor = 9
    hue = 255

  serialcmd(warpfactor, hue)
  time.sleep(3)
          

          
