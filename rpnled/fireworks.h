#ifndef __FIREWORKS_H
#define __FIREWORKS_H

#include <Arduino.h>
#include "FastLED.h"

void setupFireworks(CRGB* leds, int num_leds) ;
void fireworksLoop() ;
void startFireworks() ;
void stopFireworks() ;
void launch(uint8_t t, uint8_t hue, uint8_t sat) ;
void launch() ;

#endif
