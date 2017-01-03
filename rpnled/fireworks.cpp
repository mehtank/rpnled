#include <Arduino.h>
#include "debug.h"
#include "FastLED.h"
#include "fireworks.h"

/************************
 * Convenience macros
 ************************/

#define STARUPDATE(var, code) \
  bool var(int i) { \
    Particle *p = &(stars[i]); \
    { code } \
  }

#define REGISTER_STARS(...) \
  fn_starupdate __VA_ARGS__; \
  const fn_starupdate* starfns[] = {__VA_ARGS__};

#define XSHIFT 5
#define LED2X(x) (x << XSHIFT)
#define LED2X3(x) (x << (XSHIFT-3))
#define X2LED(x) (x >> XSHIFT)

#define GROUND LED2X(30)

/************************
 * Types
 ************************/

typedef bool fn_starupdate (int);

#define DEFAULT_COLOR CHSV(0,0,64)
typedef struct Particle {
  int16_t           x           = 0,
                    v           = 0; 

  uint8_t           g           = LED2X3(1),
                    cv          = 0,
                    substars    = 0,
                    spread      = 0;

  CHSV              color       = DEFAULT_COLOR;
  int8_t            param0      = 0;

  struct Particle*  star        = NULL;
  fn_starupdate*    fn          = NULL;

  int16_t           param1      = 0, 
                    param2      = 0;

  uint32_t          start       = 0;

} Particle;

const Particle DEFAULT_PARTICLE;

/************************
 * module variables
 ************************/

CRGB *_leds;
int _NUM_LEDS;
bool _launching = false;

#define MAXSTARS 200
int numstars = 0;
Particle stars[MAXSTARS];

/************************
 * function prototypes
 ************************/

void drawfireworks();
void updatefireworks();
void launch();
void fireworks();


/************************
 * star helpers
 ************************/

Particle* spawn(int16_t x, int16_t v, CHSV color) {
  if (numstars >= MAXSTARS)
    return NULL;

  Particle *p = &(stars[numstars++]);
  *p = DEFAULT_PARTICLE;
  p->x = x; 
  p->v = v; 
  p->color = color;
  p->start = millis();
  return p;
}

Particle* spawn(Particle *pi) {
  if (numstars >= MAXSTARS)
    return NULL;

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

typedef enum {FADE, 
              SATFADE, 
              RAINBOWFADE, 
              RAINBOWSATFADE, 
              SHIFT, 
              SPARKLE, 
              NUM_STARTYPES} StarType_t;

REGISTER_STARS( star_fade, 
                star_satfade, 
                star_rainbowfade, 
                star_rainbowsatfade, 
                star_shift, 
                star_sparkle );

STARUPDATE(star_fade,
  p->color.val -= ( p->param1 );
  if (p->color.val < 10) 
    return true;
  return false;
)

STARUPDATE(star_satfade,
  p->color.val -= ( p->param1 );
  uint16_t sat = p->color.sat + 2*p->param1;
  p->color.sat = min(sat, 255);
  if (p->color.val < 10) 
    return true;
  return false;
)

STARUPDATE(star_rainbowfade,
  if (p->color.val == 255)
    p->color.hue = random(255);
  p->color.val -= ( p->param1 );
  if (p->color.val < 10) 
    return true;
  return false;
)

STARUPDATE(star_rainbowsatfade,
  if (p->color.val == 255)
    p->color.hue = random(255);
  uint16_t sat = p->color.sat + 2*p->param1;
  p->color.sat = min(sat, 255);
  p->color.val -= ( p->param1 );
  if (p->color.val < 10) 
    return true;
  return false;
)

STARUPDATE(star_shift,
  if (p->color.sat < 2*p->param1) {
    p->color.hue = p->param2;
    p->fn = star_satfade;
  } else {
    p->color.sat -= 2*p->param1;
  }
  return false;
)

STARUPDATE(star_sparkle,
  if (millis() - p->start > p->param2) 
    return true;

  p->color.val = random(p->param1 >> 4) ? 0:255;
  p->param1++;
  return false;
)

STARUPDATE(burststar,
  for (int j = 0; j < p->substars; j++) {
    if (Particle *p2 = spawn(p)) {
      p2->substars = 0;
      p2->v = (j*2 - p->substars + 1)*p2->spread;
      p2->cv = 1;
      p2->g = 1;
    }
  }
  deletestar(i);
  return false;
)

/************************
 * general NYE functions
 ************************/

void setupFireworks(CRGB* leds, int num_leds) {
  _leds = leds;
  _NUM_LEDS = num_leds;
}

void fireworksLoop() {
    fireworks();
};

/************************
 * fireworks functions
 ************************/

void drawfireworks() {
  for (int i = 0; i < numstars; i++) {
    Particle *p = &(stars[i]);
    if ((X2LED(p->x) < _NUM_LEDS) && (p->x >= 0)) {
      _leds[X2LED(p->x)] = p->substars ? DEFAULT_COLOR : p->color;
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

void launch(Particle *pi) {
  Particle *p = spawn(pi);

  for (int i = 0; i < 4; i++) {
    if (Particle *p = spawn(GROUND, LED2X(1) + random(LED2X(1)), CHSV(40, 128, 255))) {
      p->fn = star_fade;
      p->param1 = 10;
    }
  }
}

void launch(uint8_t* buffer) {
	uint8_t t = buffer[3]; 
  Particle *p = (Particle*)(&buffer[4]);

  p->fn = starfns[t];
  p->start += millis();

  launch(p);

}

void launch(uint8_t t, uint8_t hue, uint8_t sat) {

  int16_t param1=10, param2=1000;

  switch (static_cast<StarType_t>(t)) {
    case SHIFT:
      sat = 255;
      param2 = hue ^ 128;
      break;
    case SATFADE:
      sat = 0;
      break;
    case RAINBOWSATFADE:
      sat = 0;
      break;
    case SPARKLE:
      param1 = 17;
      param2 = 1000;
      break;
  }

  Particle p;
  p.x            = GROUND;
  p.v            = LED2X(4) + random(LED2X(1));
  p.color        = CHSV(hue, sat, 255);
  p.substars     = random(5, 10);
  p.spread       = LED2X3(random(2, 10));
  p.param1       = param1;
  p.param2       = param2;

  p.start        = millis() + 1000;
  p.fn           = starfns[t];

  launch(&p);
}

void launch() {
  uint8_t hue = random(255);
  uint8_t sat = 64 + random(127);

  launch(random(NUM_STARTYPES), hue, sat);
}

void startFireworks() {
  _launching = true;
}
void stopFireworks() {
  _launching = false;
}

void fireworks() {
  uint32_t ms = millis();
  static int count = 0;

  if (_launching) {
    //if (!random(20)) launch();
    if (!(count++ % 40)) launch();
  }

  fill_solid(_leds, _NUM_LEDS, CRGB::Black);
  drawfireworks();
  updatefireworks();

  int32_t d = 10 + ms - millis();
  if (d > 0)
    delay(d);
}
