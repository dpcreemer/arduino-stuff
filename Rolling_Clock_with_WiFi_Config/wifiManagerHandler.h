
DoubleResetDetector *drd;
ProjectDisplay *wm_Display;

bool shouldSaveConfig = false;

void saveConfigCallback() {
  Serial.println("Should save config.");
  shouldSaveConfig = true;
}

void configModeCallback(WiFiManager *myWiFiManager) {
  wm_Display->drawWiFiManagerMessage(myWiFiManager);
}

void setupWiFiManager(bool forceConfig, ProjectConfig config, ProjectDisplay *theDisplay) {
  wm_Display = theDisplay;
  WiFiManager wm;
  
  Serial.println("In WiFi Manager.");
  wm.setSaveConfigCallback(saveConfigCallback);
  wm.setAPCallback(configModeCallback);

  WiFiManagerParameter timeZoneParam(PROJECT_TIME_ZONE_LABEL, "Time Zone", config.timeZone.c_str(), 60);
  
  char checkBox[] = "type=\"checkbox\"";
  char checkBoxChecked[] = "type=\"checkbox\" checked";
  char *customHtml;
  
  if (config.twentyfourHour) 
    customHtml = checkBoxChecked;
  else
    customHtml = checkBox;
  WiFiManagerParameter isTwentyfourHour(PROJECT_TIME_TWENTYFOUR_HOUR, "24H Clock", "T", 2, customHtml);

  char *customHtmlTwo;
  if (config.usDateFormat) 
    customHtmlTwo = checkBoxChecked;
  else
    customHtmlTwo = checkBox;
  WiFiManagerParameter isUsDateFormat(PROJECT_TIME_US_DATE, "US Date Format", "T", 2, customHtmlTwo);
  
  wm.addParameter(&timeZoneParam);
  wm.addParameter(&isTwentyfourHour);
  wm.addParameter(&isUsDateFormat);

  if (forceConfig) {
    drd->stop();
    if (!wm.startConfigPortal("CYD config", "cydRulez!")) {
      Serial.println("Failed to connect and hit timout. <A>");
      delay(3000);
      ESP.restart();
      delay(5000);
    }
  } else {
    if (!wm.autoConnect()) {
      Serial.println("Failed to connect and hit timeout. <B>");
      delay(3000);
      ESP.restart();
      delay(5000);
    }
  }
  
  Serial.println("Random checkpoint 6.");
  if (shouldSaveConfig) {   
    config.timeZone = String(timeZoneParam.getValue());
    config.twentyfourHour = (strncmp(isTwentyfourHour.getValue(), "T", 1) == 0);
    config.usDateFormat = (strncmp(isUsDateFormat.getValue(), "T", 1) == 0);

    config.saveConfigFile();
    drd->stop();
    ESP.restart();
    delay(5000);
  }
}