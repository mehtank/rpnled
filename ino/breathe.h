#pragma once

#define BREATHE_PIN D4
#define BREATHE_ON digitalWrite(BREATHE_PIN, LOW)
#define BREATHE_OFF digitalWrite(BREATHE_PIN, HIGH)

void breatheLoop();

