#ifndef __DEBUG_H
#define __DEBUG_H

#  define DEBUG_ENABLED

#  ifndef DEBUG_ENABLED
#    define DEBUG1(str) (void)0
#    define DEBUG2(str, var) (void)0
#  else
#    define DEBUG1(str) Serial.println(str)
#    define DEBUG2(str, var) {Serial.print(str); Serial.println(var);}
#  endif

#  define GET_MACRO(_1,_2,NAME,...) NAME
#  define DEBUG(...) GET_MACRO(__VA_ARGS__, DEBUG2, DEBUG1)(__VA_ARGS__)

#  define LED_PIN D0
#  define LED_ON digitalWrite(LED_PIN, LOW)
#  define LED_OFF digitalWrite(LED_PIN, HIGH)

#endif
