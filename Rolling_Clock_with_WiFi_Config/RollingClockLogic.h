#include "Digit.h"

TFT_eSprite sprite = TFT_eSprite(&tft);

int clockFont = 1;
int clockSize = 6;
int dateSize = 4;
int clockDatum = TL_DATUM;
uint16_t clockBackgroundColor = TFT_BLACK;
uint16_t clockTimeColor = TFT_BLUE;
uint16_t clockDateColor = TFT_ORANGE;
int prevDay = 0;

bool show24Hour = false;
bool showAmPm = true;
bool usDate = true;

Digit *digs[6];
int colons[2];
int timeY = 50;
int ampm[2];
bool ispm;

void SetupCYD() {
  Serial.println("Setup CYD");
  tft.fillScreen(clockBackgroundColor);
  tft.setTextColor(clockTimeColor, clockBackgroundColor);
  
  tft.setRotation(1);
  tft.setTextFont(clockFont);
  tft.setTextSize(clockSize);
  tft.setTextDatum(clockDatum);

  sprite.createSprite(tft.textWidth("8"), tft.fontHeight());
  sprite.setTextColor(clockTimeColor, clockBackgroundColor);
  sprite.setRotation(1);
  sprite.setTextFont(clockFont);
  sprite.setTextSize(clockSize);
  sprite.setTextDatum(clockDatum);
}

void CalculateDigitsOffsets() {
  int y = timeY;
  int width = tft.width();
  int digitWidth = tft.textWidth("8");
  int colonWidth = tft.textWidth(":");
  int left = showAmPm ? 10 : (width - digitWidth * 6 - colonWidth * 2) / 2;
  digs[0]->SetXY(left, y);
  digs[1]->SetXY(digs[0]->X() + digitWidth, y);
  colons[0] = digs[1]->X() + digitWidth;
  digs[2]->SetXY(colons[0] + colonWidth, y);
  digs[3]->SetXY(digs[2]->X() + digitWidth, y);
  colons[1] = digs[3]->X() + digitWidth;
  digs[4]->SetXY(colons[1] + colonWidth, y);
  digs[5]->SetXY(digs[4]->X() + digitWidth, y);

  ampm[0] = digs[5]->X() + digitWidth + 4;
  ampm[1] = y - 2;
}

void SetupDigits() {
  tft.fillScreen(clockBackgroundColor);
  tft.setTextFont(clockFont);
  tft.setTextSize(clockSize);
  tft.setTextDatum(clockDatum);

  for (size_t i = 0; i < 6; i++) {
    digs[i] = new Digit(0);
    digs[i]->Height(tft.fontHeight());
  }
  CalculateDigitsOffsets();
}

void DrawColons() {
  tft.setTextFont(clockFont);
  tft.setTextSize(clockSize);
  tft.setTextDatum(clockDatum);
  tft.drawChar(':', colons[0], timeY);
  tft.drawChar(':', colons[1], timeY);
}

void DrawAmPm() {
  if (showAmPm) {
    tft.setTextSize(3);
    tft.drawChar(ispm ? 'P' : 'A', ampm[0], ampm[1]);
    tft.drawChar('M', ampm[0], ampm[1] + tft.fontHeight());
  }
}
/*
void DrawADigit(Digit *digg);

void DrawADigit(Digit *digg) {
  if (digg->Value() == digg->NewValue()) {
    sprite.drawNumber(digg->Value(), 0, 0);
    sprite.pushSprite(digg->X(), digg->Y());
  } else {
    for (size_t f = 0; f <= digg->Height(); f++) {
      digg->Frame(f);
      sprite.drawNumber(digg->Value(), 0, -digg->Frame());
      sprite.drawNumber(digg->NewValue(), 0, digg->Height() - digg->Frame());
      sprite.pushSprite(digg->X(), digg->Y());
      delay(5);
    }
    digg->Value(digg->NewValue());
  }
}
*/
void DrawDigitsAtOnce() {
  char value;
  char newValue;
  tft.setTextDatum(TL_DATUM);
  tft.setTextColor(clockTimeColor, clockBackgroundColor);
  for (size_t f = 0; f <= digs[0]->Height(); f++) {
    for (size_t di = 0; di < 6; di++) {
      Digit *dig = digs[di];
      value = '0' + dig->Value();
      newValue = '0' + dig->NewValue();
      if (di == 0) {
        value = (value == '0') ? ' ' : value;
        newValue = (newValue == '0') ? ' ' : newValue;
      }
      if (dig->Value() == dig->NewValue()) {
        if (f == 0) {
          //sprite.drawNumber(dig->Value(), 0, 0);
          sprite.drawChar(value, 0, 0);
          sprite.pushSprite(dig->X(), dig->Y());
        }
      } else {
        dig->Frame(f);
        sprite.drawChar(value, 0, -f);
        sprite.drawChar(newValue, 0, dig->Height() - f);
        //sprite.drawNumber(dig->Value(), 0, -dig->Frame());
        //sprite.drawNumber(dig->NewValue(), 0, dig->Height() - dig->Frame());
        sprite.pushSprite(dig->X(), dig->Y());
      }
    }
    delay(5);
  }

  for (size_t di = 0; di < 6; di++) {
    Digit *dig = digs[di];
    dig->Value(dig->NewValue());
  }
}
/*
void DrawDigitsWithoutAnimation() {
  for (size_t di = 0; di < 6; di++) {
    Digit *dig = digs[di];
    dig->Value(dig->NewValue());
    dig->Frame(0);
    sprite.drawNumber(dig->NewValue(), 0, 0);
    sprite.pushSprite(dig->X(), dig->Y());
  }
}

void DrawDigitsOneByOne() {
  tft.setTextDatum(TL_DATUM);
  for (size_t i = 0; i < 6; i++) DrawADigit(digs[5 - i]);
}
*/
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
  time_t local = myTZ.now();
  int dd = day(local);
  int mth = month(local);
  int yr = year(local);

  tft.setTextColor(clockDateColor, clockBackgroundColor);
  if (dd != prevDay) {
    prevDay = dd;
    tft.setTextDatum(BC_DATUM);
    char buffer[50];
    if (usDate) {
      sprintf(buffer, "%02d/%02d/%d", mth, dd, yr);
    } else {
      sprintf(buffer, "%02d/%02d/%d", dd, mth, yr);
    }
    
    tft.setTextSize(dateSize);
    int h = tft.fontHeight();
    tft.fillRect(0, 210 - h, 320, h, clockBackgroundColor);
    tft.drawString(buffer, 320 / 2, 210);

    int dow = weekday(local);
    String dayNames[] = {"", "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
    tft.setTextSize(dateSize);
    tft.fillRect(0, 170 - h, 320, h, clockBackgroundColor);
    tft.drawString(dayNames[dow], 320 / 2, 170);
  }
}

void rollingClockSetup(bool is24Hour, bool isUsDate) {
  Serial.println("Rolling Clock Setup");
  show24Hour = is24Hour;
  showAmPm = !is24Hour;
  usDate = isUsDate;
  SetupCYD();
  SetupDigits();
}

void drawRollingClock() {
  ParseDigits();
  DrawDigitsAtOnce();
  DrawColons();
  DrawAmPm();
  DrawDate();
}