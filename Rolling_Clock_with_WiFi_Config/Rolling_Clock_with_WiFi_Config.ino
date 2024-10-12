#include "genericBaseProject.h"
#include "RollingClockLogic.h"

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
}
