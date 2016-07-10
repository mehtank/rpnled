import serial
import struct
import time

# Copied from: https://bitbucket.org/cmcqueen1975/cobs-python/wiki/Home
def cobs_encode(in_bytes):
    """Encode a string using Consistent Overhead Byte Stuffing (COBS).
    
    Input is any byte string. Output is also a byte string.
    
    Encoding guarantees no zero bytes in the output. The output
    string will be expanded slightly, by a predictable amount.
    
    An empty string is encoded to '\\x01'"""
    final_zero = True
    out_bytes = []
    idx = 0
    search_start_idx = 0
    for in_char in in_bytes:
        if in_char == '\x00':
            final_zero = True
            out_bytes.append(chr(idx - search_start_idx + 1))
            out_bytes.append(in_bytes[search_start_idx:idx])
            search_start_idx = idx + 1
        else:
            if idx - search_start_idx == 0xFD:
                final_zero = False
                out_bytes.append('\xFF')
                out_bytes.append(in_bytes[search_start_idx:idx+1])
                search_start_idx = idx + 1
        idx += 1
    if idx != search_start_idx or final_zero:
        out_bytes.append(chr(idx - search_start_idx + 1))
        out_bytes.append(in_bytes[search_start_idx:idx])
    out_bytes.append(chr(0))
    return ''.join(out_bytes)

def toString(data):
  prog = struct.pack( "<" + "h" * len(data), *data)
  l = len(prog) + 6
  return "$PROG" + chr(l) + prog

def encode(data):
  return cobs_encode(toString(data))
