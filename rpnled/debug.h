#ifndef __DEBUG_H
#define __DEBUG_H

#  ifndef DEBUG_ENABLED
#    define DEBUG(str, var) (void)0
#  else
#    define DEBUG(str, var) {Serial.print(str); Serial.println(var);}
#  endif

#endif
