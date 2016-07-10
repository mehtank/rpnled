#include "Arduino.h"

#ifndef __BT_H
#define __BT_H

#include "OneWireSerial.h"   //Software Serial Port

#define RxD 9  // for ProMini breakout board
#define TxD 8

#define DEBUG_ENABLED  1

#if DEBUG_ENABLED
#define DEBUG(str, var) {Serial.print(str); Serial.println(var);}
#else
#define DEBUG(str, var) (void)0
#endif

typedef void (*btCallback)(uint8_t);

void btUpdate(void);
void btSend(char c);
void btSetup(char* b, int maxlen, void (*process)(uint8_t));

#endif // ndef __BT_H
