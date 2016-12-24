#include <WebSocketsServer.h>

#ifndef __SERVER_H
#define __SERVER_H

typedef void (*ws_callback_t)(uint8_t, WStype_t, uint8_t* , size_t);

void setupSTA(char* ssid, char* password) ;
void setupAP(char* ssid, char* password) ;
void registerPage(const char* url, const char* type, String &content) ;
void setupHTTP() ;
void setupWS(ws_callback_t callback) ;
void setupMDNS(char* name) ;

void httpLoop();
void wsLoop();

#endif
