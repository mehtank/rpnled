#include <Arduino.h>
#include "breathe.h"


void breatheLoop() {
  uint32_t time = millis();

  if (time & 0x1c00) 
    analogWrite(BREATHE_PIN, 1000);
  else {
    uint32_t dt = ((time & 0x3ff) >> 2);
    if (dt & 0x80) dt = (~dt) & 0x7f;
    analogWrite(BREATHE_PIN, 1000 - (dt << 1));
  }
}
