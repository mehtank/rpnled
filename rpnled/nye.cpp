#include <Arduino.h>
#include "debug.h"
#include "FastLED.h"
#include "TimeLib.h"
#include "nye.h"

/************************
 * Types
 ************************/

#define XSHIFT 3

enum ParticleType {CONSTANT, FADE, SPARKLE, RAINBOWFADE, SHIFT, BREAK};

typedef struct {
  int x;
  int v; 
  uint8_t hue, sat, val;
  uint32_t start;
  ParticleType type;
  uint8_t param1;
  uint16_t param2;
} Particle;

/************************
 * module variables
 ************************/

CRGB *nye_leds;
int NYE_NUM_LEDS;

const int maxmortars = 1;
const int maxstars = 100;

int nummortars = 0;
int numstars = 0;

Particle mortars[maxmortars];
Particle stars[maxstars];

/************************
 * function prototypes
 ************************/

void countdown();

void drawfireworks(Particle *list, int num);
void updatefireworks(Particle *list, int *num);
void launch();
void fireworks();

/************************
 * functions
 ************************/

void setupNYE(CRGB* leds, int num_leds) {
      nye_leds = leds;
      NYE_NUM_LEDS = num_leds;
}

void nyeLoop() {
      static int last_h = 0, last_m = 0, last_s = 0;
      int h = hour(), m = minute(), s = second();

      //if (m & 1)
        //countdown();
      //else
        fireworks();
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
      nummortars = 0;
      numstars = 0;
};

void drawfireworks(Particle *list, int num) {
      for (int i = 0; i < num; i++) {
        Particle *p = &(list[i]);
        if (((p->x >> XSHIFT) < NYE_NUM_LEDS) && (p->x >= 0)) {
	  if (p->type == SPARKLE && random(p->param1))
		  continue;
          nye_leds[p->x >> XSHIFT] = CHSV(p->hue, p->sat, p->val);
	}
      }
}

void updatefireworks(Particle *list, int *num) {
      for (int i = *num-1; i >= 0; i--) {
        Particle *p = &(list[i]);
        p->x += p->v;
        if (p->x < (25 << XSHIFT)) {
          memcpy(&list[i], &list[(*num)--], sizeof(Particle));
        } else {
          p->v -= 1;
        }
      }
}

void launch() {
  Particle *p = &(mortars[nummortars++]);
  p->x = (30 << XSHIFT);
  p->v = (4 << XSHIFT) + random(1 << XSHIFT);
  p->hue = 0;
  p->sat = 0;
  p->val = 64;
  p->start = millis();
  p->type = BREAK;
  p->param1 = 4 + random(3);
  p->param2 = 1000;
  for (int i = 0; i < 4; i++) {
    if (numstars < maxstars) {
	Particle *p = &(stars[numstars++]);
	p->x = (30 << XSHIFT);
	p->v = (1 << XSHIFT) + random(1 << XSHIFT);
	p->hue = 40;
	p->sat = 128;
	p->val = 255;
	p->type = FADE;
	p->param1 = 3;
    }
  }
}

void fireworks() {
      uint32_t ms = millis();

      if (nummortars < maxmortars) 
        if (!random(10))
          launch();

      fill_solid(nye_leds, NYE_NUM_LEDS, CRGB::Black);
      drawfireworks(mortars, nummortars);
      drawfireworks(stars, numstars);
      updatefireworks(mortars, &nummortars);
      updatefireworks(stars, &numstars);

      int32_t d = 20 + ms - millis();
      DEBUG("  delay: ", d);
      if (d > 0)
	delay(d);
}
