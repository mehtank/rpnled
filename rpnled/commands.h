#include "Arduino.h"
#include "FastLED.h"

#ifndef __COMMANDS_H
#define __COMMANDS_H

#define C_RGB -1
#define C_HSV -2
#define C_HUE -3
#define C_VALUE -4
#define C_TIME -6
#define C_TIMESHIFT -7
#define C_INDEX -8
#define C_RANDOM8 -9
#define C_RANDC -10
#define C_RSHIFT -12
#define C_LSHIFT -13
#define C_BITAND -14
#define C_BITOR -15
#define C_BITXOR -16
#define C_BITNOT -17
#define C_AND -18
#define C_OR -19
#define C_NOT -20
#define C_PLUS -22
#define C_MINUS -23
#define C_TIMES -24
#define C_DIVIDE -25
#define C_MOD -26
#define C_INC -27
#define C_DEC -28
#define C_DIFF -29
#define C_EQ -31
#define C_NE -32
#define C_GT -33
#define C_GE -34
#define C_LT -35
#define C_LE -36
#define C_IFTE -38
#define C_ABS -40
#define C_MIN -41
#define C_MAX -42
#define C_REGA -44
#define C_REGB -45
#define C_REGC -46
#define C_REGD -47
#define C_REGZ -48
#define C_REGN -49
#define C_REGCYC -50
#define C_STA -52
#define C_STB -53
#define C_STC -54
#define C_STD -55
#define C_STZ -56
#define C_STN -57
#define C_STCYC -58
#define C_STAIF -60
#define C_STBIF -61
#define C_STCIF -62
#define C_STDIF -63
#define C_STZIF -64
#define C_STNIF -65
#define C_STCYCIF -66
#define C_LOOPSTART -68
#define C_LOOPREG -69
#define C_LOOPMAX -70
#define C_TDIFF -72
#define C_IDIFF -73
#define C_PRINT -75
#define REG_WIDTH 2
#define REG_IDX (1<<REG_WIDTH)
#define REG_MAX (REG_IDX - 1)
#define STACK_SIZE 64

#define MIN(X,Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X,Y) (((X) > (Y)) ? (X) : (Y))

CRGB runCmd(int16_t *cmd, uint8_t cmdlen, uint32_t time, uint16_t index, uint8_t rnd);

#endif // ndef __COMMANDS_H
