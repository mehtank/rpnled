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


/************************
 * LED strip parameters
 ************************/

#define NUM_LEDS 300
#define PROGLEN 42
#define BUFLEN PROGLEN*2 + 6

const int STRIP_PIN = 5;

// Create a buffer for holding the colors (3 bytes per color).
CRGB leds[NUM_LEDS];

// Create a buffer for holding the program
int16_t program[PROGLEN] = {4, C_TIMESHIFT, C_INDEX, 3, C_LSHIFT, C_MINUS, 255, C_BITAND, C_HUE};
uint8_t proglen = 9;

/************************
 * Wireless parameters
 ************************/

// WiFi AP parameters
char* ap_ssid = "ESP_xxxxxxxx";
char* ap_password = "";

// WiFi STA parameters
char* sta_ssid = 
  "DG1670AF2"; //gitignore  "...";
char* sta_password = 
  "DG1670A6ACDF2"; //gitignore  "...";

char* mDNS_name = "led_xxxxxxxx";

String html;
String js;

void setup() {
  Serial.begin(115200);
  DEBUG("Started serial");

  pinMode(LED_PIN, OUTPUT);    //Set LED pin
  LED_OFF;                     //Turn off LED

  FastLED.addLeds<NEOPIXEL, STRIP_PIN>(leds, NUM_LEDS);

  sprintf(ap_ssid, "ESP_%08X", ESP.getChipId());
  sprintf(mDNS_name, "led_%08X", ESP.getChipId());

  LED_ON;
  setupSTA(sta_ssid, sta_password);
  setupAP(ap_ssid, ap_password);
  LED_OFF;

  setupFile();
  html = loadFile("/index.html");
  js = loadFile("/led.js");
  registerPage("/", "text/html", html);
  registerPage("/led.js", "text/javascript", js);

  setupHTTP();
  setupWS(webSocketEvent);
  setupMDNS(mDNS_name);
}

uint8_t loopcnt = 0;

void loop() {
  // Handle server stuff
  wsLoop();
  httpLoop();

  // Update the colors.
  uint32_t time = millis();
  if (loopcnt == 25) LED_ON;
  if (loopcnt++ > 50) {
    LED_OFF;
    // DEBUG("Time = ", time);
    loopcnt = 0;
  }

  uint8_t rnd = random(256);
  for(uint16_t i = 0; i < NUM_LEDS; i++)   
    leds[i] = runCmd(program, proglen, time, i, rnd);
  FastLED.show(); // display this frame
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * buffer, size_t rxc) {
    switch(type) {
        case WStype_DISCONNECTED:
	    DEBUG("Connection started : ", num);
            break;
        case WStype_CONNECTED: 
            // IPAddress ip = webSocket.remoteIP(num);
            // Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
            DEBUG("Web socket connected");

            // send message to client
            // webSocket.sendTXT(num, "Connected");
            break;
        case WStype_TEXT:
            Serial.printf("[%u] get text: %s\n", num, buffer);
	    DEBUG("On connection : ", num);
	    DEBUG("  Got buffer of length : ", rxc);

	    if (rxc > 5 && !strncmp((char*)buffer, "$PROG", 5) && rxc == buffer[5]+1) {
	      memcpy((char*)program, &buffer[6], rxc-5);
	      proglen = (rxc-6)/2;

	      DEBUG("  Set program length : ", proglen);
	      for (int i = 0; i < proglen; i++)
		DEBUG("    Set program string : ", program[i]);
	    }
	    break;
    }
}

