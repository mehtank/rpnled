from commands import *

alloff = [
  0, 0, 0, C_RGB
]

allon = [
  255, 255, 255, C_RGB
]

rainbow = [
    C_TIME, 4, C_RSHIFT,
    C_INDEX, 2, C_LSHIFT,
  C_MINUS, 256, C_MOD,
  255,
  255,
  C_HSV
]

colorwave2 = [
  C_TIME, 6, C_RSHIFT, C_INDEX, 1, C_RSHIFT, C_MINUS, 256, C_MOD, 255, C_TIME, 4, C_RSHIFT, C_INDEX, 4, C_LSHIFT, C_PLUS, 511, C_MOD, 255, C_MINUS, C_ABS, C_HSV
]

colorwave = [
  C_TIME, 6, C_RSHIFT, 255, C_TIME, 4, C_RSHIFT, C_INDEX, 4, C_LSHIFT, C_PLUS, 511, C_MOD, 255, C_MINUS, C_ABS, C_HSV
]

weird = [ 
  C_TIME, 8, C_RSHIFT, 
  255, 
    C_TIME, 2, C_RSHIFT, C_INDEX, 4, C_LSHIFT, C_PLUS, 512, C_MOD, 
    511,    
    C_TIME, 4, C_RSHIFT, C_INDEX, 4, C_LSHIFT, C_PLUS, 512, C_MOD, 
    C_MINUS, 
  C_MAX, 256, C_MINUS, C_HSV
]

weird2 = [ 
  8, C_TIMESHIFT,
  255, 
    2, C_TIMESHIFT, C_INDEX, 4, C_LSHIFT, C_PLUS, 511, C_BITAND, 
    511,    
    4, C_TIMESHIFT, C_INDEX, 4, C_LSHIFT, C_PLUS, 511, C_BITAND, 
    C_MINUS, 
  C_MAX, 256, C_MINUS, C_HSV
]

calrunner2 = [
  5,C_TIMESHIFT,C_INDEX,C_PLUS,32,C_BITAND,145,39,C_IFTE, 4,C_TIMESHIFT, 127, C_BITAND, C_INDEX, 127, C_BITAND, C_EQ, 0, 255, C_IFTE, 255, C_HSV
]

calrunner = [
  6,C_TIMESHIFT,C_INDEX,C_PLUS,32,C_BITAND,C_STA,
C_REGA,145,39,C_IFTE, 4,C_TIMESHIFT, 127, C_BITAND, C_INDEX, 127, C_BITAND, C_EQ, 0, 255, C_IFTE, 255, C_HSV
]

calblink = [
  5,C_TIMESHIFT,C_INDEX,C_PLUS,32,C_BITAND,C_STA,
C_REGA,145,39,C_IFTE, C_RANDC, 1023, C_BITAND, C_INDEX, C_EQ, 0, 255, C_IFTE, 255, C_HSV
]

cal = [
  5,C_TIMESHIFT,C_INDEX,C_PLUS,32,C_BITAND,C_STA,
C_REGA,0,253,C_IFTE,C_REGA,50,181,C_IFTE,C_REGA,98,21,C_IFTE, C_RGB
]

ucla = [
10,C_TIMESHIFT,C_INDEX,5,C_RSHIFT,C_PLUS,1,C_BITAND,C_STA,
C_REGA,145,39,C_IFTE,C_REGA,188,255,C_IFTE,C_REGA,191,255,C_IFTE, C_HSV
]

blink = [
  C_INDEX, C_REGA, C_REGA, 255, C_BITXOR, C_IFTE, C_STA, 
255,0,C_REGA, C_HSV
]

weirdwave = [ 
  6, C_TIMESHIFT, C_INDEX, C_MINUS,
  255, 
    2, C_TIMESHIFT, C_INDEX, 4, C_LSHIFT, C_PLUS, 511, C_BITAND, 
    511,    
    4, C_TIMESHIFT, C_INDEX, 4, C_LSHIFT, C_PLUS, 511, C_BITAND, 
    C_MINUS, 
  C_MAX, 256, C_MINUS, C_HSV
]

weirdwave2 = [ 
  6, C_TIMESHIFT, C_INDEX, 1, C_RSHIFT, C_MINUS,
  255, 
    2, C_TIMESHIFT, C_INDEX, 4, C_LSHIFT, C_PLUS, 511, C_BITAND, 
    511,    
    4, C_TIMESHIFT, C_INDEX, 4, C_LSHIFT, C_PLUS, 511, C_BITAND, 
    C_MINUS, 
  C_MAX, 256, C_MINUS, C_HSV
]

usa = [
      C_INDEX, 6, C_TIMESHIFT, C_PLUS, 32, C_BITAND, 
      0, 
      160, 
    C_IFTE, 
      C_INDEX, 7, C_TIMESHIFT, C_MINUS, 7, C_BITAND, 
      255, 
      0, 
    C_IFTE, 
    255, 
  C_HSV 
]

usa2 = [
  C_INDEX, 6, C_TIMESHIFT, C_PLUS, C_STA,
      C_REGA, 32, C_BITAND, 
      0, 
      160, 
    C_IFTE, 
      C_INDEX, 7, C_TIMESHIFT, C_MINUS, 7, C_BITAND, 
      255, C_REGA, 31, C_BITAND, 16, C_DIFF, 15, C_MIN, 4, C_LSHIFT, C_MINUS,
      0, 
    C_IFTE, 
    255, 
  C_HSV 
]

usa3 = [
  C_INDEX, 6, C_TIMESHIFT, C_PLUS, C_STA,
      C_REGA, 32, C_BITAND, 
      0, 
      160, 
    C_IFTE, 
      C_INDEX, 7, C_TIMESHIFT, C_MINUS, 7, C_BITAND, 
      255, C_REGA, 31, C_BITAND, 1, C_LSHIFT, 32, C_DIFF, 17, C_MINUS, 0, C_MAX, 4, C_LSHIFT, C_MINUS,
      0, 
    C_IFTE, 
    255, 
  C_HSV 
]

usa4 = [
  C_INDEX, 6, C_TIMESHIFT, C_PLUS, C_STA,
      C_REGA, 32, C_BITAND, 
      0, 
      160, 
    C_IFTE, 
    255, C_REGA, 31, C_BITAND, 1, C_LSHIFT, 32, C_DIFF, 17, C_MINUS, 0, C_MAX, 4, C_LSHIFT, C_MINUS,
    255, 
  C_HSV 
]
