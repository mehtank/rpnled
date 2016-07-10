from encode import encode
from patterns import *

target = "COM11"

def send(data):
  sock = serial.Serial(target)
  time.sleep(0.1)
  sock.write(encode(data))
  sock.close()
