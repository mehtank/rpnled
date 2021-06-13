#include "debug.h"
#include "FastLED.h"
#include "TimeLib.h"
#include "nye.h"

/************************
 * module variables
 ************************/

CRGB *nye_leds;
int NYE_NUM_LEDS;

/************************
 * function prototypes
 ************************/

void countdown();

/************************
 * general NYE functions
 ************************/

void setupNYE(CRGB* leds, int num_leds) {
  nye_leds = leds;
  NYE_NUM_LEDS = num_leds;
}

void nyeLoop() {
  countdown();
};

void countdown() {
  static int last_s = 0;
  int s = second();

  if (last_s == s) {
    int fadeby = (s > 39) ? 7 : 1;
    for(uint16_t i = 0; i < NYE_NUM_LEDS; i++)
        nye_leds[i].fadeToBlackBy( fadeby );
    if (s < 50) delay(10);
  } else {
    CRGB color = CRGB::White;
    int s10 = s%10;

    if (s < 50) {
      if (s < 10)
        color = CRGB::Yellow;
      else if (s < 20)
        color = CRGB::Red;
      else if (s < 30)
        color = CRGB::Purple;
      else if (s < 40)
        color = CRGB::Blue;
      else if (s < 50)
        color = CRGB::Cyan;

      for (int j = 0; j <= s10; j++) {
        int start = 87 - 6*s10 + 12*j;
        for (int i = start; i < start + 7; i++) {
          nye_leds[i] = color;
        }
      }
    } else {
      int d = 6+s10;
      for (int j = 0; j < (10-s10); j++) {
        int start = 90 - d/2 - d*(9-s10) + 2*d*j;
        for (int i = start; i < start + d + 1; i++) {
          nye_leds[i] = color;
        }
      }
    }
  }
  last_s = s;
};
