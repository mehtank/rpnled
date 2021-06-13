#include "mydebug.h"
#include <myota.h>
#include <ESPAsyncWebServer.h>
#include "FastLED.h"
#include "commands.h"
#include "files.h"
#include "fireworks.h"


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
int16_t program[PROGLEN] = {4, C_TIMESHIFT, C_INDEX, 3, C_LSHIFT, C_MINUS, 255, C_BITAND, 255, 96, C_HSV};
uint8_t proglen = 11;

enum ProgramState {STOPPED, RUNNING, ONCE, REDRAW, FIREWORKS};
ProgramState state = RUNNING;

uint8_t loopcnt = 0;
uint32_t offat = 0;

/************************
 * Wireless parameters
 ************************/

static const char* SSID       = "torii";
static const char* PASS       = "6283185307";
static const char* MDNM       = "tableled";

// WiFi AP parameters
char* ap_ssid = "ESP_xxxxxxxx";
char* ap_password = "";

/************************
 * Web server
 ************************/

AsyncWebServer httpServer(80);
AsyncWebSocket ws("/ws");

#define STATIC(page, type, pg)                                          \
  httpServer.on(page, HTTP_GET, [](AsyncWebServerRequest *request){     \
    AsyncWebServerResponse *response =                                  \
    request->beginResponse_P(200, "text/" type, pg##_gz, pg##_gz_len);\
    response->addHeader("Content-Encoding", "gzip");                    \
    request->send(response);                                            \
  });

/************************
 * Handle websocket
 ************************/

void webSocketEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, 
             void * arg, uint8_t *data, size_t len){

  switch (type) {
    case WS_EVT_CONNECT:
      //client connected
      DEBUGF("ws[%s][%u] connect\n", server->url(), client->id());
      client->printf("Hello Client %u :)", client->id());
      client->ping();
      break;
    case WS_EVT_DISCONNECT:
      //client disconnected
      DEBUGF("ws[%s][%u] disconnect: %u\n", server->url(), client->id());
      break;
    case WS_EVT_ERROR:
      //error was received from the other end
      DEBUGF("ws[%s][%u] error(%u): %s\n", server->url(), client->id(), *((uint16_t*)arg), (char*)data);
      break;
    case WS_EVT_PONG:
      //pong message was received (in response to a ping request maybe)
      DEBUGF("ws[%s][%u] pong[%u]: %s\n", server->url(), client->id(), len, (len)?(char*)data:"");
      break;
    case WS_EVT_DATA:
      //data packet
      AwsFrameInfo * info = (AwsFrameInfo*)arg;
      if(info->final && info->index == 0 && info->len == len){
        //the whole message is in a single frame and we got all of it's data
        DEBUGF("ws[%s][%u] %s-message[%llu]: ", server->url(), client->id(), (info->opcode == WS_TEXT)?"text":"binary", info->len);
        if(info->opcode == WS_TEXT){
          data[len] = 0;
          DEBUGF("%s\n", (char*)data);
        } else {
          for(size_t i=0; i < info->len; i++){
            DEBUGF("%02x ", data[i]);
          }
          DEBUGF("\n");
        }
        if(info->opcode == WS_TEXT)
          client->text("I got your text message");
        else
          client->binary("I got your binary message");
      } else {
        //message is comprised of multiple frames or the frame is split into multiple packets
        if(info->index == 0){
          if(info->num == 0)
            DEBUGF("ws[%s][%u] %s-message start\n", server->url(), client->id(), (info->message_opcode == WS_TEXT)?"text":"binary");
          DEBUGF("ws[%s][%u] frame[%u] start[%llu]\n", server->url(), client->id(), info->num, info->len);
        }

      DEBUGF("ws[%s][%u] frame[%u] %s[%llu - %llu]: ", server->url(), client->id(), info->num, (info->message_opcode == WS_TEXT)?"text":"binary", info->index, info->index + len);
      if(info->message_opcode == WS_TEXT){
        data[len] = 0;
        DEBUGF("%s\n", (char*)data);
      } else {
        for(size_t i=0; i < len; i++){
          DEBUGF("%02x ", data[i]);
        }
        DEBUGF("\n");
      }

      if((info->index + len) == info->len){
        DEBUGF("ws[%s][%u] frame[%u] end[%llu]\n", server->url(), client->id(), info->num, info->len);
        if(info->final){
          DEBUGF("ws[%s][%u] %s-message end\n", server->url(), client->id(), (info->message_opcode == WS_TEXT)?"text":"binary");
          if(info->message_opcode == WS_TEXT)
            client->text("I got your text message");
          else
            client->binary("I got your binary message");
        }
      }
    }
  }
}

void handleString(int rxc, uint8_t * buffer) {

	    if (rxc > 5 && !strncmp((char*)buffer, "$PROG", 5) && rxc == 2*buffer[5]+6) {
	      memcpy((char*)program, &buffer[6], rxc-5);
	      proglen = (rxc-6)/2;

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
	    } else if (!strncmp((char*)buffer, "$FW", 3)) {
              state = FIREWORKS;
              fireworksEvent(buffer, rxc); 
	    } else if (!strncmp((char*)buffer, "$OFF", 4)) {
              fill_solid(leds, NUM_LEDS, CRGB::Black);
              state = REDRAW;
        }
}

/************************
 * Set up pins, LEDs, wifi
 ************************/

void setup() {
  DEBUG_START;
  DEBUG("Started serial");

  pinMode(LED_PIN, OUTPUT);    //Set LED pin
  LED_OFF;                     //Turn off LED

  FastLED.addLeds<NEOPIXEL, STRIP_PIN>(leds, NUM_LEDS);

  sprintf(ap_ssid, "ESP_%08X", ESP.getChipId());

  LED_ON;
  LED_OFF;

  ota_init(SSID, PASS, MDNM);

  STATIC("/", "html", index_html);
  STATIC("/led.js", "javascript", led_js);
  ws.onEvent(webSocketEvent);
  httpServer.addHandler(&ws);

  httpServer.begin();

  setupFireworks(leds, NUM_LEDS);
}

/************************
 * Do LED loop
 ************************/

void loop() {
  static uint32_t time = 0;
  static uint32_t last = 0;

  // Handle server stuff
  ota_loop();

  //breatheLoop();

  // Update the colors.
  time = millis();

  if ((time - last) > 1100) {
    last = time;
    // time_t t = NTP.getTime(); // forces resync
    //DEBUG("hour: ", hour());
    //DEBUG("minute: ", minute());
    //DEBUG("second: ", second());
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
    case FIREWORKS:
      fireworksLoop();
      FastLED.show(); // display this frame
      break;
  }
}
