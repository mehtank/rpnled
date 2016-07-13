#include "FastLED.h"
#include "commands.h"
#include "mybt.h"
#include <avr/wdt.h>

#define NUM_LEDS 300
#define NUM_STRIPS 2
#define PROGLEN 42
#define BUFLEN PROGLEN*2 + 6

// Create a buffer for holding the colors (3 bytes per color).
CRGB leds[NUM_LEDS];
CLEDController *controllers[NUM_STRIPS];

// Create a buffer to receive data from bluetooth
char buffer[BUFLEN];
int16_t program[PROGLEN] = {4, C_TIMESHIFT, C_INDEX, 3, C_LSHIFT, C_MINUS, 255, C_BITAND, C_HUE};
uint8_t proglen = 9;

void process(uint8_t rxc) {
  if (rxc > 0) {
    DEBUG("RX'ed bytes: ", rxc);
  }
  if (rxc > 5 && !strncmp(buffer, "$PROG", 5) && rxc == buffer[5]+1) {
    memcpy((char*)program, &buffer[6], rxc-5);
    proglen = (rxc-6)/2;

    DEBUG("Set program length : ", proglen);
    for (int i = 0; i < proglen; i++)
      DEBUG("Set program string : ", program[i]);
  }
}

void setup() {
  wdt_disable();
  Serial.begin(115200);
  DEBUG("Started serial ", 0);
  btSetup(buffer, BUFLEN, &process);
  DEBUG("Started bluetooth ", 0);

  controllers[0] = &FastLED.addLeds<NEOPIXEL, 3>(leds, NUM_LEDS);
  controllers[1] = &FastLED.addLeds<NEOPIXEL, 2>(leds, NUM_LEDS);
  wdt_enable(WDTO_1S);
}

uint8_t loopcnt = 0;

void loop() {
  // feed watchdog
  wdt_reset();

  // Update the colors.
  uint32_t time = millis();
  if (loopcnt++ == 25) {
    //DEBUG("Time = ", time);
    //btSend(65);
    loopcnt = 0;
  }

  uint8_t rnd = random(256);
  for(uint16_t i = 0; i < NUM_LEDS; i++)   
    leds[NUM_LEDS-i-1] = runCmd(program, proglen, time, i, rnd);
  controllers[0]->showLeds(); // display this frame

  for(uint16_t i = NUM_LEDS; i < 2*NUM_LEDS; i++)   
    leds[i-NUM_LEDS] = runCmd(program, proglen, time, i, rnd);
  controllers[1]->showLeds(); // display this frame

  btUpdate();
}
