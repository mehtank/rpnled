#include <Arduino.h>

#include <Hash.h>
#include <FS.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <ESP8266mDNS.h>

#include "FastLED.h"
#include "commands.h"

#define DEBUG_ENABLED
#include "debug.h"
#include "file.h"
#include "server.h"

#define NUM_LEDS 300
#define PROGLEN 42
#define BUFLEN PROGLEN*2 + 6

const int STRIP_PIN = 5;

// Create a buffer for holding the colors (3 bytes per color).
CRGB leds[NUM_LEDS];

// Create a buffer for holding the program
int16_t program[PROGLEN] = {4, C_TIMESHIFT, C_INDEX, 3, C_LSHIFT, C_MINUS, 255, C_BITAND, C_HUE};
uint8_t proglen = 9;

/*
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
*/

void setup() {
  Serial.begin(115200);
  DEBUG("Started serial");
  pinMode(LED_PIN, OUTPUT);    //Set LED pin
  LED_OFF;                     //Turn off LED

  FastLED.addLeds<NEOPIXEL, STRIP_PIN>(leds, NUM_LEDS);
}

uint8_t loopcnt = 0;

void loop() {
  // Update the colors.
  uint32_t time = millis();
  if (loopcnt == 25) LED_ON;
  if (loopcnt++ > 50) {
    LED_OFF;
    DEBUG("Time = ", time);
    loopcnt = 0;
  }

  uint8_t rnd = random(256);
  for(uint16_t i = 0; i < NUM_LEDS; i++)   
    leds[i] = runCmd(program, proglen, time, i, rnd);
  FastLED.show(); // display this frame
}
