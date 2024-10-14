#include "ClockDigit.h"
#include "fonts/FiraMono_Bold32pt7b.h"
#include "fonts/FiraMono_Bold12pt7b.h"
#include "fonts/FiraMono_Bold16pt7b.h"
#include "fonts/FiraSans_Regular12pt7b.h"
#include "fonts/FiraSans_Bold24pt7b.h"

#define FONT_DIGIT &FiraMono_Bold32pt7b
#define FONT_AMPM &FiraMono_Bold12pt7b
#define FONT_DATE &FiraMono_Bold16pt7b
#define FONT_MSG &FiraSans_Regular12pt7b
#define FONT_DAY &FiraSans_Bold24pt7b

uint16_t COLOR_BG = TFT_BLACK;
uint16_t COLOR_TIME = TFT_BLUE;
uint16_t COLOR_DATE = TFT_ORANGE;

int YPOS_TIME = 40;

int DATUM = TL_DATUM;

const String DAY_NAMES[] = {"", "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

TFT_eSprite sprite = TFT_eSprite(&tft);
int prevDay = 0;
uint32_t msgTimer = 0;
bool show24Hour = false;
bool showAmPm = true;
bool usDate = true;

Digit *digs[6];
int colons[2];
int ampm[2];
bool ispm;

void SetupCYD() {
  Serial.println("Setup CYD");
  tft.fillScreen(COLOR_BG);
  tft.setTextColor(COLOR_TIME, COLOR_BG);
  
//  tft.setRotation(1);
  tft.setFreeFont(FONT_DIGIT);
  tft.setTextDatum(DATUM);

  sprite.createSprite(tft.textWidth("8"), tft.fontHeight()*.75);
  sprite.setTextColor(COLOR_TIME, COLOR_BG);
  sprite.setRotation(1);
  sprite.setFreeFont(FONT_DIGIT);
  sprite.setTextDatum(DATUM);
}

void CalculateDigitsOffsets() {
  int digitWidth = sprite.textWidth("8");
  int colonWidth = sprite.textWidth(":") + 10;
  int left = showAmPm ? 10 : (tft.width() - digitWidth * 6 - colonWidth * 2) / 2;

  digs[0]->SetXY(left, YPOS_TIME);
  digs[1]->SetXY(digs[0]->X() + digitWidth, YPOS_TIME);
  colons[0] = digs[1]->X() + digitWidth;
  digs[2]->SetXY(colons[0] + colonWidth, YPOS_TIME);
  digs[3]->SetXY(digs[2]->X() + digitWidth, YPOS_TIME);
  colons[1] = digs[3]->X() + digitWidth;
  digs[4]->SetXY(colons[1] + colonWidth, YPOS_TIME);
  digs[5]->SetXY(digs[4]->X() + digitWidth, YPOS_TIME);

  ampm[0] = digs[5]->X() + digitWidth + 8;
  ampm[1] = YPOS_TIME + 2;
}

void SetupDigits() {
  for (size_t i = 0; i < 6; i++) {
    digs[i] = new Digit(0);
    digs[i]->Height(tft.fontHeight());
  }
  CalculateDigitsOffsets();
}

void DrawColons() {
  int yOffset = -10;
  tft.setFreeFont(FONT_DIGIT);
  tft.setTextDatum(DATUM);
  tft.drawString(":", colons[0], YPOS_TIME + yOffset);
  tft.drawString(":", colons[1], YPOS_TIME + yOffset);
}

void DrawAmPm() {
  if (showAmPm) {
    tft.setFreeFont(FONT_AMPM);
    tft.drawString(ispm ? "P" : "A", ampm[0], ampm[1]);
    tft.drawString("M", ampm[0], ampm[1] + tft.fontHeight());
  }
}

void DrawDigitsAtOnce() {
  int yOffset = -6;
  
  tft.setTextDatum(TL_DATUM);
  tft.setTextColor(COLOR_TIME, COLOR_BG);

  for (size_t f = 0; f <= digs[0]->Height(); f++) {
    for (size_t di = 0; di < 6; di++) {
      if (digs[di]->Value() == digs[di]->NewValue()) {
        if (f == 0) {
          sprite.drawNumber(digs[di]->NewValue(), 0, yOffset);
          sprite.pushSprite(digs[di]->X(), digs[di]->Y());
        }
      } else {
        sprite.drawNumber(digs[di]->Value(), 0, yOffset-f);
        sprite.drawNumber(digs[di]->NewValue(), 0, yOffset + digs[di]->Height() - f);
        sprite.pushSprite(digs[di]->X(), digs[di]->Y());
      }
    }
    delay(5);
  }

  for (size_t di = 0; di < 6; di++) digs[di]->Value(digs[di]->NewValue());
}

void ParseDigits() {
  time_t local = myTZ.now();
  digs[0]->NewValue((show24Hour ? hour(local) : hourFormat12(local)) / 10);
  digs[1]->NewValue((show24Hour ? hour(local) : hourFormat12(local)) % 10);
  digs[2]->NewValue(minute(local) / 10);
  digs[3]->NewValue(minute(local) % 10);
  digs[4]->NewValue(second(local) / 10);
  digs[5]->NewValue(second(local) % 10);
  ispm = isPM(local);
}

void DrawDate() {
  int y = 110;
  time_t local = myTZ.now();
  int dd = day(local);
  int mth = month(local);
  int yr = year(local);

  tft.setTextColor(COLOR_DATE, COLOR_BG);
  if (dd != prevDay) {
    prevDay = dd;

    tft.setTextDatum(TC_DATUM);
    tft.setFreeFont(FONT_DAY);
    tft.fillRect(0, y, tft.width(), tft.fontHeight(), COLOR_BG);
    tft.drawString(DAY_NAMES[weekday(local)], tft.width() / 2, y);
    y+= tft.fontHeight();
    
    tft.setFreeFont(FONT_DATE);
    char buffer[50]; 
    sprintf(buffer, "%02d/%02d/%d", usDate ? mth : dd, usDate ? dd : mth, yr);
    tft.fillRect(0, y, tft.width(), tft.fontHeight(), COLOR_BG);
    tft.drawString(buffer, tft.width() / 2, y);
  }
}

void statusMessage(String msg) {
  tft.setFreeFont(FONT_MSG);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.setTextDatum(BR_DATUM);
  tft.drawString(msg, tft.width(), tft.height());
  msgTimer = millis() + 10000;
  tft.setTextDatum(DATUM);
}

void clearMessage() {
  tft.fillRect(0, tft.height()-25, tft.width(), tft.height(), COLOR_BG);
  msgTimer = 0;
}

void buttonPress() {
  buttonPressed = false;
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP().toString());
  statusMessage("IP: " + WiFi.localIP().toString());
}

void rollingClockSetup(bool is24Hour, bool isUsDate) {
  Serial.println("Rolling Clock Setup");
  show24Hour = is24Hour;
  showAmPm = !is24Hour;
  usDate = isUsDate;
  SetupCYD();
  SetupDigits();
  DrawColons();
}

void drawRollingClock() {
  ParseDigits();
  DrawDigitsAtOnce();
  DrawAmPm();
  DrawDate();
}