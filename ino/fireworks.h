#pragma once

#include "FastLED.h"

void setupFireworks(CRGB* leds, int num_leds) ;
void fireworksLoop() ;
void fireworksEvent(size_t rxc, char* buffer);
