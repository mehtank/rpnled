from bluetooth import *
from patterns import *
from encode import encode
import time
import IPython

target = "00:13:01:07:20:58"
target = "20:14:05:21:29:71"

def send(data):
  sock.send(encode(data))

sock=BluetoothSocket( RFCOMM )
sock.connect((target, 1))
time.sleep(0.1)

IPython.embed()
