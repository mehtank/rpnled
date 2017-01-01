#include <Arduino.h>

#include <Hash.h>
#include <FS.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include <TimeLib.h>
#include <NtpClientLib.h>

#include "FastLED.h"
#include "commands.h"

#include "breathe.h"
#include "debug.h"
#include "file.h"
#include "server.h"


/************************
 * LED strip parameters
 ************************/

#define NUM_LEDS 300
#define FOR_LEDS(var) for(uint16_t var = 0; var < NUM_LEDS; var++)   
#define PROGLEN 200
#define BUFLEN PROGLEN*2 + 6

const int STRIP_PIN = 5;

// Create a buffer for holding the colors (3 bytes per color).
CRGB leds[NUM_LEDS];

// Create a buffer for holding the program
int16_t program[PROGLEN] = {4, C_TIMESHIFT, C_INDEX, 3, C_LSHIFT, C_MINUS, 255, C_BITAND, C_HUE};
uint8_t proglen = 9;

enum ProgramState {STOPPED, RUNNING, ONCE, REDRAW, NYE};
ProgramState state = NYE;

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

char* mDNS_name = "lrled";

String html;
String js;

/************************
 * NTP callbacks
 ************************/

void onSTAGotIP(WiFiEventStationModeGotIP ipInfo) {
	DEBUG("Got IP: ", ipInfo.ip.toString().c_str());
	NTP.begin("tick.ucla.edu", 1, true);
	NTP.setInterval(20);
	NTP.setTimeZone(-8);
}

void onSTADisconnected(WiFiEventStationModeDisconnected event_info) {
	DEBUG("Disconnected from SSID: ", event_info.ssid.c_str());
	DEBUG("Reason: ", event_info.reason);
}

void setupNTP() {
	static WiFiEventHandler e1, e2;

	NTP.onNTPSyncEvent([](NTPSyncEvent_t ntpEvent) {
		if (ntpEvent) {
			DEBUG("Time Sync error: ");
			if (ntpEvent == noResponse)
				DEBUG("  NTP server not reachable");
			else if (ntpEvent == invalidAddress)
				DEBUG("  Invalid NTP server address");
		}
		else {
			DEBUG("Got NTP time: ", NTP.getTimeDateString(NTP.getLastNTPSync()));
		}
	});
	WiFi.onEvent([](WiFiEvent_t e) {
		Serial.printf("Event wifi -----> %d\n", e);
	});
	e1 = WiFi.onStationModeGotIP(onSTAGotIP);// As soon WiFi is connected, start NTP Client
	e2 = WiFi.onStationModeDisconnected(onSTADisconnected);
}

/************************
 * Set up pins, LEDs, wifi
 ************************/

void setup() {
  Serial.begin(115200);
  DEBUG("Started serial");

  pinMode(BREATHE_PIN, OUTPUT);    //Set LED pin
  BREATHE_OFF;                     //Turn off LED

  pinMode(LED_PIN, OUTPUT);    //Set LED pin
  LED_OFF;                     //Turn off LED


  FastLED.addLeds<NEOPIXEL, STRIP_PIN>(leds, NUM_LEDS);

  sprintf(ap_ssid, "ESP_%08X", ESP.getChipId());
  //sprintf(mDNS_name, "led_%08X", ESP.getChipId());

  LED_ON;
  // setupSTA(sta_ssid, sta_password);
  setupAP(ap_ssid, ap_password);
  LED_OFF;

  setupNTP();
  setupFile();
  html = loadFile("/index.html");
  DEBUG("  loaded html: ", html.length());
  js = loadFile("/led.js");
  DEBUG("  loaded js: ", js.length());
  registerPage("/", "text/html", html);
  registerPage("/led.js", "text/javascript", js);

  setupHTTP();
  setupWS(webSocketEvent);
  setupMDNS(mDNS_name);
  ArduinoOTA.setHostname(mDNS_name);
  ArduinoOTA.begin();
}

/************************
 * Do LED loop
 ************************/

uint8_t loopcnt = 0;
uint32_t offat = 0;

void loop() {
  static uint32_t time = 0;
  static uint32_t last = 0;

  // Handle server stuff
  wsLoop();
  httpLoop();
  ArduinoOTA.handle();
  //breatheLoop();

  // Update the colors.
  time = millis();

  if ((time - last) > 1100) {
    last = time;
    // time_t t = NTP.getTime(); // forces resync
    DEBUG("hour: ", hour());
    DEBUG("minute: ", minute());
    DEBUG("second: ", second());
  }

  if (time > offat) LED_OFF;

  switch (state) {
    case STOPPED:
      break;

    case ONCE:
      state = STOPPED;
    case RUNNING: {
      uint8_t rnd = random(256);
      for(uint16_t i = 0; i < NUM_LEDS; i++)   
        leds[i] = runCmd(program, proglen, time, i, rnd);
      FastLED.show(); // display this frame
      break;
    }

    case REDRAW:
      FastLED.show(); // display this frame
      break;
    case NYE:
      runNYE();
      FastLED.show(); // display this frame
      break;
  }
}

/************************
 * NYE loop
 ************************/
void runNYE() {
      static int last_h = 0, last_m = 0, last_s = 0;
      static uint32_t last_ms = 0;
      int h = hour(), m = minute(), s = second();
      uint32_t ms = millis();

      countdown();
};


void countdown() {
      static int last_s = 0;
      int s = second();

      if (last_s == s) {
        int fadeby = (s > 39) ? 7 : 1;
        FOR_LEDS(i)
            leds[i].fadeToBlackBy( fadeby );
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
              leds[i] = color;
            }
          }
        } else {
          int d = 6+s10;
          for (int j = 0; j < (10-s10); j++) {
            int start = 90 - d/2 - d*(9-s10) + 2*d*j;
            for (int i = start; i < start + d + 1; i++) {
              leds[i] = color;
            }
          }
        }
      }
      last_s = s;
};

/************************
 * Handle websocket
 ************************/

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
        case WStype_BIN:
	    DEBUG("On connection : ", num);
	    DEBUG("  Got buffer of length : ", rxc);
	    DEBUG("  Length byte: ", buffer[5]);
            for (int i = 0; i < rxc; i++)
              DEBUG("    char : ", buffer[i]);

	    if (rxc > 5 && !strncmp((char*)buffer, "$PROG", 5) && rxc == 2*buffer[5]+6) {
	      memcpy((char*)program, &buffer[6], rxc-5);
	      proglen = (rxc-6)/2;

	      DEBUG("  Set program length : ", proglen);
	      for (int i = 0; i < proglen; i++)
		DEBUG("    Set program string : ", program[i]);
              state = RUNNING;

	    } else if (rxc > 5 && !strncmp((char*)buffer, "$RGB", 4) && rxc == 3*((uint16_t*)(buffer))[2]+6) {
	      memcpy((char*)leds, &buffer[6], rxc-6);
              state = REDRAW;

	    } else if (!strncmp((char*)buffer, "$BLINK", 6)) {
              LED_ON;
              offat = millis() + 500;

	    } else if (!strncmp((char*)buffer, "$PAUSE", 6)) {
              state = STOPPED;
	    } else if (!strncmp((char*)buffer, "$START", 6)) {
              state = RUNNING;
	    } else if (!strncmp((char*)buffer, "$NEXT", 5)) {
              state = ONCE;
	    } else if (!strncmp((char*)buffer, "$NYE", 4)) {
              state = NYE;
	    } else if (!strncmp((char*)buffer, "$OFF", 4)) {
              fill_solid(leds, NUM_LEDS, CRGB::Black);
              state = REDRAW;
            }
	    break;
        case WStype_TEXT:
            Serial.printf("[%u] get text: %s\n", num, buffer);
            break;
        default:
            DEBUG("Unknown type : ", type);
            DEBUG("Unknown len : ", rxc);
    }
}

