#include "mydebug.h"
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

#define TICK 10

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

  uint8_t           ticks       = 0;
  CHSV              color       = DEFAULT_COLOR;

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
  if ((numstars >= MAXSTARS) || (!pi))
    return NULL;

  Particle *p = &(stars[numstars++]);
  *p = *pi;
  p->start = millis();

  if (pi->star) {
    Particle *star = new Particle;
    *star = *(pi->star);
    p->star = star;
  }

  return p;
}

void deletestar(int i) {
  Particle *p = &(stars[i]);
  if (p->star) delete p->star;
	memcpy(p, &stars[--numstars], sizeof(Particle));
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
              STROBE, 
              STROBEFADE, 
              COMET, 
              GLITTER, 
              NUM_STARTYPES} StarType_t;

REGISTER_STARS( star_fade, 
                star_satfade, 
                star_rainbowfade, 
                star_rainbowsatfade, 
                star_shift, 
                star_sparkle,
                star_strobe,
                star_strobefade,
                star_comet,
                star_glitter
              );

STARUPDATE(star_fade,
  if (p->color.val < p->param1) 
    return true;
  p->color.val -= ( p->param1 );
  return false;
)

STARUPDATE(star_satfade,
  p->color.val -= ( p->param1 );
  uint16_t sat = p->color.sat + 2*p->param1;
  p->color.sat = min(sat, (uint16_t) 255);
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
  p->color.sat = min(sat, (uint16_t) 255);
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

STARUPDATE(star_strobe,
  if (p->color.val == 255 && p->color.sat == 0)
    return true;

  if (millis() - p->start > p->param1 + random(p->param2)) {
    p->color.sat = 0;
    p->color.val = 255;
  }

  return false;
)

STARUPDATE(star_strobefade,
  if (p->color.val == 255 && p->color.sat == 0) {
    return true;
  } else if (p->color.val < p->param1) {
    p->color.sat = 0;
    p->color.val = 255;
  } else {
    p->color.val -= ( p->param1 );
  }
  return false;
)

STARUPDATE(star_comet,
  if (millis() - p->start > p->param2) 
    return true;

  if (Particle *p2 = spawn(p->x, 0, p->color)) {
    p2->fn = star_fade;
    p2->param1 = random(p->param1, 4*p->param1);
  }
  return false;
)

STARUPDATE(star_glitter,
  if (millis() - p->start > p->param2) 
    return true;

  if (Particle *p2 = spawn(p->x, 0, p->color)) {
    p2->fn = star_strobefade;
    p2->param1 = random(p->param1, 4*p->param1);
  }
  return false;
)

STARUPDATE(burststar,
  for (int j = 0; j < p->substars; j++) {
    if (Particle *p2 = spawn(p->star)) {
      p2->x = p->x;
      p2->v = (j*2 - p->substars + 1)*p->spread;
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
      _leds[X2LED(p->x)] = p->color;
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
    else if ((p->substars) && (millis() > p->start + TICK * p->ticks))
      burststar(i);
    else if (p->fn && p->fn(i) && !p->substars)
      deletestar(i);
  }
}

void launch(Particle *mortar, Particle *star) {
  mortar->star = star;
  Particle *p = spawn(mortar);

  for (int i = 0; i < 4; i++) {
    if (Particle *p = spawn(GROUND, LED2X(1) + random(LED2X(1)), CHSV(40, 128, 255))) {
      p->fn = star_fade;
      p->param1 = 10;
    }
  }
}

void launch(Particle *pi) {
  Particle mortar;
  mortar = *pi;
  mortar.color = DEFAULT_COLOR;
  mortar.fn = star_fade;
  mortar.param1 = 1;

  pi->substars = 0;

  launch(&mortar, pi);
}

void launch(uint8_t t, uint8_t hue, uint8_t sat) {

  Particle p;
  p.x            = GROUND;
  p.v            = LED2X(4) + random(LED2X(1));
  p.color        = CHSV(hue, sat, 255);
  p.substars     = random(5, 10);
  p.spread       = LED2X3(random(2, 10));

  p.start        = millis();
  p.ticks        = 100;
  p.fn           = starfns[t];
  p.param1       = 10;
  p.param2       = 1000;

  switch (static_cast<StarType_t>(t)) {
    case SHIFT:
      p.color.sat = 255;
      p.param2 = p.color.hue ^ 128;
      break;
    case SATFADE:
      p.color.sat = 0;
      break;
    case RAINBOWSATFADE:
      p.color.sat = 0;
      break;
    case SPARKLE:
      p.param1 = 17;
      break;
    case STROBE:
      p.substars <<= 1;
      p.spread >>= 1;
      p.color.val = 64;
      p.param1 = 500;
      break;
    case COMET:
      p.substars >>= 1;
      p.param2 = 1500;
      break;
    case GLITTER:
      p.substars >>= 1;
      break;
  }

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

  int32_t d = TICK + ms - millis();
  if (d > 0)
    delay(d);
}

void fireworksEvent(uint8_t* buffer, uint8_t rxc) {
	if (!strncmp((char*)buffer, "$FWSTART", 8)) {
		startFireworks();
	} else if (!strncmp((char*)buffer, "$FWSTOP", 7)) {
		stopFireworks();
	} else if (!strncmp((char*)buffer, "$FWGO", 5)) {
		stopFireworks();
		launch();
	} else if (!strncmp((char*)buffer, "$FW", 3)) {
		stopFireworks();
		if (rxc == 6) {
			launch(buffer[3], buffer[4], buffer[5]);
		} else if (rxc == sizeof(Particle) + 4) {
			uint8_t t = buffer[3]; 
			Particle *p = (Particle*)(&buffer[4]);
			p->fn = starfns[t];
			launch(p);
    } else if (rxc == sizeof(Particle) * 2 + 5) {
			uint8_t mt = buffer[3]; 
			uint8_t st = buffer[4 + sizeof(Particle)*2];
			Particle *mortar = (Particle*)(&buffer[4]);
			Particle *star = (Particle*)(&buffer[4 + sizeof(Particle)]);
			mortar->fn = starfns[mt];
			star->fn = starfns[st];
			launch(mortar, star);
		} 
	}
}
