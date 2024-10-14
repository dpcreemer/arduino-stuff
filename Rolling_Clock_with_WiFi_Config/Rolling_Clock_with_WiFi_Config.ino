#include "genericBaseProject.h"
#include "ClockLogic.h"

bool first = true;

void setup() {
  Serial.begin(115200);

  baseProjectSetup();
  rollingClockSetup(projectConfig.twentyfourHour, projectConfig.usDateFormat);  
}

void loop() {
  baseProjectLoop();
  if (first || secondChanged()) {
    drawRollingClock();
    first = false;
  }
  if (buttonPressed) buttonPress();
  if (msgTimer != 0 && millis() > msgTimer) clearMessage();
}
