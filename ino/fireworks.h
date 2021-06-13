#pragma once

#include "FastLED.h"

void setupFireworks(CRGB* leds, int num_leds) ;
void fireworksLoop() ;
void fireworksEvent(uint8_t* buffer, uint8_t rxc);
