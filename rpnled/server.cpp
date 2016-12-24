#include <Arduino.h>
#include "server.h"
#include "debug.h"

#include <ESP8266WiFi.h>
#include <WiFiClient.h>

#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>

#include <ESP8266mDNS.h>

#define STA_MAXTRIES 10

ESP8266WebServer httpServer = ESP8266WebServer(80);
WebSocketsServer wsServer = WebSocketsServer(81);

//
// Setup //
//

void setupSTA(char* ssid, char* password) {
    DEBUG("Connecting to STA");
    WiFi.begin(ssid, password);

    int tries = 0;
    while (WiFi.status() != WL_CONNECTED) {
        if (tries++ > STA_MAXTRIES) {
            DEBUG("  giving up.");
            return;
        }
        delay(500);
        DEBUG("  ... waiting");
    }

    IPAddress myIP = WiFi.localIP();
    DEBUG("STA IP address: ", myIP.toString());
}

void setupAP(char* ssid, char* password) {
    WiFi.softAP(ssid, password);

    IPAddress myIP = WiFi.softAPIP();
    DEBUG("AP IP address: ", myIP.toString());
}

void registerPage(const char* url, const char* type, String &content) {
    httpServer.on(url,  [&type, &content]() { httpServer.send(200, type, content); }); 
}

void setupHTTP() {
    httpServer.begin();
}

void setupWS(ws_callback_t callback) {
    // start webSocket server
    wsServer.begin();
    wsServer.onEvent(callback);
}

void setupMDNS(char* name) {
    if(MDNS.begin(name)) {
        // Add services to mDNS
        MDNS.addService("http", "tcp", 80);
        MDNS.addService("ws", "tcp", 81);
        DEBUG("mDNS responder started");
    } else {
        DEBUG("mDNS failed\n");
    }
}

void httpLoop() {
	httpServer.handleClient();                                                       
}

void wsLoop() {
	wsServer.loop();
}
