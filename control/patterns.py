from commands import *
from makepattern import *

alloff = makepattern([
  0, 0, 0, C_RGB
])

allon = makepattern([
  255, 255, 255, C_RGB
])

solid = makepattern([
  '$hue_0_h', '$sat_255_s', '$val_255_v', C_HSV
])

rainbow = makepattern([
    C_TIME, '$speed_4', C_RSHIFT,
    C_INDEX, '$width_2', C_LSHIFT,
  C_MINUS, 255, C_BITAND,
  '$sat_255_s',
  '$value_255_v',
  C_HSV
])

colorwave = makepattern([
  C_TIME, '$colorspeed_6', C_RSHIFT,
  255, 
    C_TIME, '$pulsespeed_4', C_RSHIFT, 
    C_INDEX, '$pulsewidth_4', C_LSHIFT, 
    C_PLUS, 511, C_MOD, 
  255, C_MINUS, C_ABS, 
  C_HSV
])

rainbowwave = makepattern([
    C_TIME, '$colorspeed_6', C_RSHIFT,
    C_INDEX, '$colorwidth_1', C_LSHIFT,
  C_MINUS, 255, C_BITAND,
  255, 
    C_TIME, '$pulsespeed_4', C_RSHIFT, 
    C_INDEX, '$pulsewidth_4', C_LSHIFT, 
    C_PLUS, 511, C_MOD, 
  255, C_MINUS, C_ABS, 
  C_HSV
])

weird = makepattern([ 
  '$colorspeed_8', C_TIMESHIFT,
  255, 
    '$pulsespeeda_2', C_TIMESHIFT, C_INDEX, '$pulsewidth_4', C_LSHIFT, C_PLUS, 511, C_BITAND, 
    511,    
    '$pulsewidthb_4', C_TIMESHIFT, C_INDEX, '$pulsewidth_4', C_LSHIFT, C_PLUS, 511, C_BITAND, 
    C_MINUS, 
  C_MAX, 256, C_MINUS, C_HSV
])

calrunner2 = makepattern([
  5,C_TIMESHIFT,C_INDEX,C_PLUS,32,C_BITAND,145,39,C_IFTE, 4,C_TIMESHIFT, 127, C_BITAND, C_INDEX, 127, C_BITAND, C_EQ, 0, 255, C_IFTE, 255, C_HSV
])

calrunner = makepattern([
  6,C_TIMESHIFT,C_INDEX,C_PLUS,32,C_BITAND,C_STA,
C_REGA,145,39,C_IFTE, 4,C_TIMESHIFT, 127, C_BITAND, C_INDEX, 127, C_BITAND, C_EQ, 0, 255, C_IFTE, 255, C_HSV
])

calblink = makepattern([
  5,C_TIMESHIFT,C_INDEX,C_PLUS,32,C_BITAND,C_STA,
C_REGA,145,39,C_IFTE, C_RANDC, 1023, C_BITAND, C_INDEX, C_EQ, 0, 255, C_IFTE, 255, C_HSV
])

cal = makepattern([
  5,C_TIMESHIFT,C_INDEX,C_PLUS,32,C_BITAND,C_STA,
C_REGA,0,253,C_IFTE,C_REGA,50,181,C_IFTE,C_REGA,98,21,C_IFTE, C_RGB
])

ucla = makepattern([
10,C_TIMESHIFT,C_INDEX,5,C_RSHIFT,C_PLUS,1,C_BITAND,C_STA,
C_REGA,145,39,C_IFTE,C_REGA,188,255,C_IFTE,C_REGA,191,255,C_IFTE, C_HSV
])

blink = makepattern([
  C_INDEX, C_REGA, C_REGA, 255, C_BITXOR, C_IFTE, C_STA, 
255,0,C_REGA, C_HSV
])

weirdwave = makepattern([ 
  6, C_TIMESHIFT, C_INDEX, C_MINUS,
  255, 
    2, C_TIMESHIFT, C_INDEX, 4, C_LSHIFT, C_PLUS, 511, C_BITAND, 
    511,    
    4, C_TIMESHIFT, C_INDEX, 4, C_LSHIFT, C_PLUS, 511, C_BITAND, 
    C_MINUS, 
  C_MAX, 256, C_MINUS, C_HSV
])

weirdwave2 = makepattern([ 
  6, C_TIMESHIFT, C_INDEX, 1, C_RSHIFT, C_MINUS,
  255, 
    2, C_TIMESHIFT, C_INDEX, 4, C_LSHIFT, C_PLUS, 511, C_BITAND, 
    511,    
    4, C_TIMESHIFT, C_INDEX, 4, C_LSHIFT, C_PLUS, 511, C_BITAND, 
    C_MINUS, 
  C_MAX, 256, C_MINUS, C_HSV
])

usa = makepattern([
      C_INDEX, 6, C_TIMESHIFT, C_PLUS, 32, C_BITAND, 
      '$hue1_0', 
      '$hue2_160', 
    C_IFTE, 
      C_INDEX, 7, C_TIMESHIFT, C_MINUS, 7, C_BITAND, 
      255, 
      0, 
    C_IFTE, 
    255, 
  C_HSV 
])

usa2 = makepattern([
  C_INDEX, 6, C_TIMESHIFT, C_PLUS, C_STA,
      C_REGA, 32, C_BITAND, 
      '$hue1_0', 
      '$hue2_160', 
    C_IFTE, 
      C_INDEX, 7, C_TIMESHIFT, C_MINUS, 7, C_BITAND, 
      255, C_REGA, 31, C_BITAND, 16, C_DIFF, 15, C_MIN, 4, C_LSHIFT, C_MINUS,
      0, 
    C_IFTE, 
    255, 
  C_HSV 
])

usa3 = makepattern([
  C_INDEX, 6, C_TIMESHIFT, C_PLUS, C_STA,
      C_REGA, 32, C_BITAND, 
      '$hue1_0', 
      '$hue2_160', 
    C_IFTE, 
      C_INDEX, 7, C_TIMESHIFT, C_MINUS, 7, C_BITAND, 
      255, C_REGA, 31, C_BITAND, 1, C_LSHIFT, 32, C_DIFF, 17, C_MINUS, 0, C_MAX, 4, C_LSHIFT, C_MINUS,
      0, 
    C_IFTE, 
    255, 
  C_HSV 
])

usa4 = makepattern([
  C_INDEX, 6, C_TIMESHIFT, C_PLUS, C_STA,
      C_REGA, 32, C_BITAND, 
      '$hue1_0', 
      '$hue2_160', 
    C_IFTE, 
    255, C_REGA, 31, C_BITAND, 1, C_LSHIFT, 32, C_DIFF, 17, C_MINUS, 0, C_MAX, 4, C_LSHIFT, C_MINUS,
    255, 
  C_HSV 
])

dots = makepattern([
      C_INDEX, '$colorshift_6', C_TIMESHIFT, C_PLUS, 1, '$width_5', C_LSHIFT, C_BITAND,
      '$hue1_0',
      '$hue2_96',
    C_IFTE,
    255,
    C_INDEX, '$gapshift_7', C_TIMESHIFT, C_MINUS, '$gap_7', C_BITAND, 0, 255, C_IFTE,
  C_HSV
])

vday = makepattern([
  C_INDEX, '$dotshift_7', C_TIMESHIFT, C_MINUS, '$dotpattern_10', C_BITAND, C_STB, 
      C_REGB, 
      '$hue1_220_h', 
      '$hue2_0_h', 
    C_IFTE, 
      C_REGB, 
      C_INDEX, '$pulseshift_4', C_TIMESHIFT, C_PLUS, 
        63, C_BITAND, 
        1, C_LSHIFT, 
        64, C_DIFF, 
        33, C_MINUS, 
        0, C_MAX, 
        3, C_LSHIFT, 
      255, 
    C_IFTE, 
    '$value_255', 
  C_HSV
])
