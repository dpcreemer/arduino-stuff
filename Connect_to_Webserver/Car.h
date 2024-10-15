class Car {
private:
  int m_speed;
  char m_gear;
  int m_charge;

public:
  Car(int speed, char gear, int charge);
  int Speed();
  int Speed(int speed);
  char Gear();
  char Gear(char gear);
  int Charge();
  int Charge(int charge);
  String PostString();
};