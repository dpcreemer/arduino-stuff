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

  void drawWiFiManagerMessage(WiFiManager *myWiFiManager) {
    Serial.println("Entered Config Mode.");
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawCentreString("Entered Config Mode", screenCenterX, 5, 2);
    tft.drawString("Connected to the following WiFi AP:", 5, 28, 2);
    tft.setTextColor(TFT_BLUE, TFT_BLACK);
    tft.drawString(myWiFiManager->getConfigPortalSSID(), 20, 48, 2);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString("Password:", 5, 64, 2);
    tft.setTextColor(TFT_BLUE, TFT_BLACK);
    tft.drawString("fswcasbcic", 20, 82, 2);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString("IP Address:", 5, 100, 2);
    tft.setTextColor(TFT_BLUE, TFT_BLACK);
    tft.drawString(WiFi.localIP().toString(), 20, 118, 2);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    tft.drawString("If it doesn't AutoConnect, use this IP:", 5, 146, 2);
    tft.setTextColor(TFT_BLUE, TFT_BLACK);
    tft.drawString(WiFi.softAPIP().toString(), 20, 164, 2);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
  }
};