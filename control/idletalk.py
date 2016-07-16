from encode import encode
from patterns import *
import serial
import time

target = "COM11"

def send(data):
  sock = serial.Serial(target)
  time.sleep(0.1)
  e = encode(data)
  chunk = 40
  for s in (e[i:i+chunk] for i in range(0,len(e),chunk)):
    sock.write(s)
    time.sleep(0.1)
  sock.close()
