#include <Arduino.h>
#include "Timeout.h"


void Timeout::Reset() {
    startTime = millis();
}

bool Timeout::HasElapsed(const unsigned int delay) {
    unsigned long currentTime = millis();
    unsigned long elapsedTime = currentTime - startTime;

    // Serial.print("Elapsed time: ");
    // Serial.println(elapsedTime);

    return elapsedTime >= delay;
}
