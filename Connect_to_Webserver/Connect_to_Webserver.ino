#include <SPI.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <TFT_eSPI.h>
#include "Car.h"

const char* SSID = "CYD is Cool";
const char* PASSWORD = "cydRulez!";
const int16_t COLOR_BG = TFT_BLACK;
const String ARGS[] = {"alpha", "beta", "gamma", "deta", "epsilon"};
const String VALUES[] = {"gato", "pero", "mono", "pajaro", "elefante"};
const char* serverName = "http://192.168.4.1:80/";
char postBuffer[60];

HTTPClient http;
int buttonPressed = false;
Car *car = new Car(50, 'D', 90);

void IRAM_ATTR buttonInt() {
  buttonPressed = true;
  Serial.println("button pressed.");
}

void buttonPress() {
  buttonPressed = false;
  Serial.println("Connect to Server for Post.");
  http.begin(serverName);
  Serial.println("Connected. Making Post.");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
//  String post = ARGS[0] + "=" + VALUES[random(5)];
//  for (int i = 1; i < 5; i++) post += "&" + ARGS[i] + "=" + VALUES[random(5)]; 
//  int resp = http.POST(post);
  sprintf(postBuffer, "speed=%d&gear=%c&charge=%d", car->Speed(), car->Gear(), car->Charge());
  Serial.print("Post: ");
  Serial.println(postBuffer);
  int resp = http.POST(postBuffer);
  Serial.printf("Resp code: %d\n", resp);
  http.end();
}

void updateDisplay() {
  http.begin(serverName);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  sprintf(postBuffer, "speed=%d&gear=%c&charge=%d", car->Speed(), car->Gear(), car->Charge());
  int resp = http.POST(postBuffer);
  http.end();
}

void setup() {
  Serial.begin(115200);
  
  attachInterrupt(0, buttonInt, FALLING);

  WiFi.begin(SSID, PASSWORD);
  Serial.print("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  
  Serial.println("\nConnected to WiFi");
  Serial.println("IP: " + WiFi.localIP().toString() + "\n");
  
  http.begin(serverName);
  int resp = http.GET();

  if (resp > 0) {
    Serial.printf("HTTP Response code: %d\n\n", resp);
    String payload = http.getString();
    Serial.println(payload);
  } else {
    Serial.printf("Connection failed: %d\n", resp);
  }
}

void loop() {
  if (buttonPressed) buttonPress();
  car->Update();
  updateDisplay();
  delay(400);
}
