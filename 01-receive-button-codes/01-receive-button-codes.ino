#include <avr/wdt.h>
#include "Driver.h"
#include "AppFunction.h"

void setup() {
  MyApp.Init();
}

void loop() {
  MyApp.Receive();
}
