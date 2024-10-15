#include "Car.h"

Car::Car(int speed, char gear, int charge) {
  m_speed = speed;
  m_gear = gear;
  m_charge = charge;
}

int Car::Speed() {
  return m_speed;
}

int Car::Speed(int speed) {
  m_speed = speed;
  return m_speed;
}

char Car::Gear() {
  return m_gear;
}

char Car::Gear(char gear) {
  m_gear = gear;
  return m_gear;
}

int Car::Charge() {
  return m_charge;
}

int Car::Charge(int charge) {
  m_charge = charge;
  return m_charge;
}

String Car::PostString() {
  String rv = "speed=" + String(m_speed);
  rv += "&gear=" + m_gear;
  rv += "&charge=" + String(m_charge);
  return rv;
}