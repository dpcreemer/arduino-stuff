#include "projectDisplay.h"
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

class CheapYellowDisplay : public ProjectDisplay {
public:
  void displaySetup() {
    Serial.println("CYD display setup.");
    setWidth(320);
    setHeight(240);

    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);
    tft.invertDisplay(true);
  }

  void drawMessage(String msg, int x, int y, uint16_t color) {
    tft.setTextColor(color, TFT_BLACK);
    tft.drawString(msg, x, y, 2);
  }

  void drawWiFiManagerMessage(WiFiManager *myWiFiManager) {
    Serial.println("Entered Config Mode.");
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawCentreString("Entered Config Mode", tft.width()/2, 5, 4);
    drawMessage("Connect to the following WiFi:", 5, 40, TFT_WHITE);
    drawMessage(myWiFiManager->getConfigPortalSSID(), 20, 60, TFT_BLUE);
    drawMessage("Password:", 5, 80, TFT_WHITE);
    drawMessage("cydRulez!", 20, 100, TFT_BLUE);
    drawMessage("If it doesn't AutoConnect, use this IP:", 5, 140, TFT_WHITE);
    drawMessage(WiFi.softAPIP().toString(), 20, 160, TFT_BLUE);
  }
};