#define ESP_DRD_USE_SPIFFS true

#include <WiFi.h>
#include <FS.h>
#include "SPIFFS.h"
#include <WiFiManager.h>
#include <ESP_DoubleResetDetector.h>
#include <ezTime.h>
#include <ArduinoJson.h>
#include "projectConfig.h"
#include "projectDisplay.h"
#include "wifiManagerHandler.h"
#include "cheapYellowLCD.h"

#define DRD_TIMEOUT 10
#define DRD_ADDRESS 0

ProjectConfig projectConfig;
CheapYellowDisplay cyd;
ProjectDisplay *projectDisplay = &cyd;

Timezone myTZ;
int buttonPressed = false;

void IRAM_ATTR buttonInt() {
  buttonPressed = true;
  Serial.println("button pressed.");
}

void baseProjectSetup() {
  projectDisplay->displaySetup();

  bool forceConfig = false;
  attachInterrupt(0, buttonInt, FALLING);

  drd = new DoubleResetDetector(DRD_TIMEOUT, DRD_ADDRESS);
  if (drd->detectDoubleReset()) {
    Serial.println(F("Forcing config mode as there was a double reset detected. :) "));
    forceConfig = true;
  }
  
  bool spiffsInitSuccess = SPIFFS.begin(false) || SPIFFS.begin(true);
  if (!spiffsInitSuccess) {
    Serial.println("SPIFFS initialization failed.");
    while (1) yield();
  }

  Serial.println("\r\nInitialization done.");
  if (!projectConfig.fetchConfigFile()) forceConfig = true;

  Serial.println("Going to WiFi Manager.");
  setupWiFiManager(forceConfig, projectConfig, projectDisplay);
  Serial.println("Back from WiFi Manager.");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Waiting for time sync.");

  waitForSync();

  Serial.println("");
  Serial.println("UTC:             " + UTC.dateTime());

  myTZ.setLocation(projectConfig.timeZone);
  Serial.print(projectConfig.timeZone);
  Serial.print(F(":     "));
  Serial.println(myTZ.dateTime());
  Serial.println("---------------------------");
}

void baseProjectLoop() {
  drd->loop();
}