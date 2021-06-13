#ifndef __FIREWORKS_H
#define __FIREWORKS_H

#include <Arduino.h>
#include "FastLED.h"

void setupFireworks(CRGB* leds, int num_leds) ;
void fireworksLoop() ;
void fireworksEvent(uint8_t* buffer, uint8_t rxc);

#endif
