from websocket import create_connection
from patterns import *
import IPython

localname = "lrled"
target = "ws://%s.local:81/" % localname

def send(data):
    ws = create_connection(target)
    ws.send_binary(data)
    ws.close()

IPython.embed()
