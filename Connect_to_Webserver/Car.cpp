#include "Car.h"
#include <cstdio>

Car::Car(int speed, char gear, int charge) {
  m_speed = speed;
  m_gear = gear;
  m_charge = charge;
  m_targetSpeed = speed;
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

void Car::Update() {
  randomSeed(millis());
  if (random(10) == 0 && m_charge > 50) 
    m_targetSpeed = 20 * random(4);
  else if (m_charge < 10)
    m_targetSpeed = 0;
  m_gear = (m_targetSpeed + m_speed > 0) ? 'D' : 'P';
  if (m_speed != m_targetSpeed)
    m_speed += (m_targetSpeed > m_speed) ? 2 : -2;
  if (m_gear == 'D') m_charge -= 1;
  if (m_gear == 'P' && m_charge < 100) m_charge += 2;
  if (m_charge > 100) m_charge = 100;
  if (m_charge < 0) m_charge = 0;
}