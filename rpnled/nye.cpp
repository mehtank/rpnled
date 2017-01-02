#include <Arduino.h>
#include "debug.h"
#include "FastLED.h"
#include "TimeLib.h"
#include "nye.h"

/************************
 * Types
 ************************/

typedef bool (*fn_starupdate) (int);

typedef struct {
  int x, v; 
  uint8_t g, cv;
  CHSV color;
  uint8_t substars;
  uint32_t start;
  fn_starupdate fn;
  int32_t param1, param2, param3;
} Particle;

const Particle DEFAULT_PARTICLE = {0,0, 1,0, CHSV(0,0,0), 0, 0, 0, 0,0,0};
const CHSV DEFAULT_COLOR = CHSV(0,0,64);

/************************
 * Convenience macros
 ************************/

#define STARUPDATE(var) bool var(int i) { Particle *p = &(stars[i]);

#define XSHIFT 3
#define LED2X(x) (x << XSHIFT)
#define X2LED(x) (x >> XSHIFT)

#define GROUND LED2X(30)

/************************
 * module variables
 ************************/

CRGB *nye_leds;
int NYE_NUM_LEDS;

const int maxstars = 100;
int numstars = 0;
Particle stars[maxstars];

/************************
 * function prototypes
 ************************/

void countdown();

void drawfireworks();
void updatefireworks();
void launch();
void fireworks();


/************************
 * star helpers
 ************************/

Particle* spawn(int x, int v, CHSV color) {
  Particle *p = &(stars[numstars++]);
  *p = DEFAULT_PARTICLE;
  p->x = x; 
  p->v = v; 
  p->color = color;
  p->start = millis();
  return p;
}

Particle* spawn(Particle *pi) {
  Particle *p = &(stars[numstars++]);
  *p = *pi;
  return p;
}

void deletestar(int i) {
	memcpy(&stars[i], &stars[--numstars], sizeof(Particle));
}

/************************
 * star updates
 ************************/

STARUPDATE(star_fade)
  p->color.val -= ( p->param1 );
  if (p->color.val < 10) 
    return true;
  return false;
}

STARUPDATE(star_rainbowfade)
  if (p->color.val == 255)
    p->color.hue = random(255);
  p->color.val -= ( p->param1 );
  if (p->color.val < 10) 
    return true;
  return false;
}

STARUPDATE(star_shift)
  if (millis() - p->start > p->param2) {
    p->color.hue = p->param1;
    p->fn = star_fade;
    p->param1 = 5;
  };
  return false;
}

STARUPDATE(star_sparkle)
  if (millis() - p->start > p->param2) 
    return true;

  p->color.val = random(p->param1 >> 4) ? 0:255;
  p->param1++;
  return false;
}

STARUPDATE(burststar)
  int spread = random(1, 5) << (XSHIFT-2);
  for (int j = 0; j < p->substars; j++) {
    if (numstars < maxstars) {
      Particle *p2 = spawn(p);
      p2->substars = 0;
      p2->v = (j*2 - p->substars + 1)*spread;
      p2->cv = 1;
    }
  }
  deletestar(i);
  return false;
}

/************************
 * general NYE functions
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
  numstars = 0;
};

/************************
 * fireworks functions
 ************************/

void drawfireworks() {
  for (int i = 0; i < numstars; i++) {
    Particle *p = &(stars[i]);
    if ((X2LED(p->x) < NYE_NUM_LEDS) && (p->x >= 0)) {
      nye_leds[X2LED(p->x)] = p->substars ? DEFAULT_COLOR : p->color;
    }
  }
}

void updatefireworks() {
  for (int i = numstars-1; i >= 0; i--) {
    Particle *p = &(stars[i]);
    p->x += p->v;
    p->v -= p->g + p->cv*(p->v / 4);
    if (p->x < 0) 
      deletestar(i);
    else if (p->substars) {
      if (millis() > p->start) burststar(i);
    } else if (p->fn && p->fn(i))
      deletestar(i);
  }
}

void launch() {
  uint8_t hue = random(255);
  uint8_t sat = 64 + random(127);

  Particle *p = spawn(GROUND, LED2X(4) + random(LED2X(1)), CHSV(hue, sat, 255));
  p->substars = random(5, 10);
  p->start += 1000;
  p->fn = star_sparkle;
  p->param1 = 10;
  p->param2 = 1000;

  for (int i = 0; i < 4; i++) {
    if (numstars < maxstars) {
      Particle *p = spawn(GROUND, LED2X(1) + random(LED2X(1)), CHSV(40, 128, 255));
      p->fn = star_fade;
      p->param1 = 10;
    }
  }
}

void fireworks() {
      uint32_t ms = millis();
      static int count = 0;

      //if (!random(20)) launch();
      if (!(count++ % 60)) launch();

      fill_solid(nye_leds, NYE_NUM_LEDS, CRGB::Black);
      drawfireworks();
      updatefireworks();

      int32_t d = 10 + ms - millis();
      DEBUG("  delay: ", d);
      if (d > 0)
	delay(d);
}
