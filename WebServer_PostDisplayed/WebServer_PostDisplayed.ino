#include <SPI.h>
#include <FS.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <TFT_eSPI.h>

const char* ssid = "Seven is Enough";
const char* password = "fswcasbcic";
const int8_t BACKGROUND_COLOR = TFT_BLACK;

WebServer server(80);
TFT_eSPI tft = TFT_eSPI();

void drawFillBar(int32_t x, int32_t y, int8_t fill) {
  tft.fillRect(x, y, tft.width() - 2 * x, 8, TFT_LIGHTGREY);
  tft.fillRect(x, y, (tft.width() - 2 * x) * fill/100, 8, TFT_GREEN);
}

void drawGuage(String label, int16_t value, int16_t max, int32_t x, int32_t y) {
  tft.fillCircle(x, y, 58, BACKGROUND_COLOR);
  tft.drawArc(x, y, 53, 51, 60, 300, TFT_LIGHTGREY, BACKGROUND_COLOR);
  tft.drawArc(x, y, 54, 50, 60, 60 + (240 * value / max), TFT_GREEN, BACKGROUND_COLOR);
  tft.drawString(String(value), x, y-20, 6);
  tft.drawString(label, x, y + 30, 4);
}

void showGear(String gear) {
  int16_t box = 32;
  int16_t y = 10;
  int16_t x = tft.width() - box - y;
  tft.setTextColor(BACKGROUND_COLOR, TFT_YELLOW);
  tft.fillRoundRect(x, y, box, box, 4, TFT_YELLOW);
  tft.drawString(gear, x + (box/2), y+5, 4);
  Serial.printf("Gear: %s\n", gear);
  tft.setTextColor(TFT_YELLOW, BACKGROUND_COLOR);
}

void showSpeed(int16_t speed) {
  drawGuage("Speed", speed, 150, 60, 120);
  Serial.printf("Speed: %d mph\n");
//  int16_t y = 100;
//  tft.fillRect(0, y, 240, 26, BACKGROUND_COLOR);
//  tft.drawString("Speed: " + String(speed) + " mph", tft.width()/2, y, 4);
//  drawFillBar(20, y+30, speed/1.5);
}

void showBattery(int8_t battery) {
  drawGuage("Charge", battery, 100, 180, 120);
  Serial.printf("Charge: %d\%\n", battery);
//  int16_t y = 160;
//  tft.fillRect(0, y, 240, 26, BACKGROUND_COLOR);
//  tft.drawString("Battery: " + String(battery) + "%", tft.width()/2, y, 4);
//  drawFillBar(20, y+30, battery);
}

bool is_authenticated() {
  Serial.println("Enter is_authenticated");
  if (server.hasHeader("Cookie")) {
    String cookie = server.header("Cookie");
    Serial.println("  Found cookie: " + cookie);
    if (cookie.indexOf("ESPSESSIONID=1") != -1) {
      Serial.println("  Authentication Successful");
      return true;
    }
  }
  Serial.println("  Authentication failed");
  return false;
}

void handleLogin() {
  String msg;
  Serial.println("Entering handleLogin");
  if (server.hasHeader("Cookie")) {
    String cookie = server.header("Cookie");
    Serial.println("  Found cookie: " + cookie);
  }
  if (server.hasArg("DISCONNECT")) {
    Serial.println("  Disconnecting");
    server.sendHeader("Location", "/login");
    server.sendHeader("Cache-Control", "no-cache");
    server.sendHeader("Set-Cookie", "ESPSESSIONID=0");
    server.send(301);
    return;
  }
  if (server.hasArg("USERNAME") && server.hasArg("PASSWORD")) {
    if (server.arg("USERNAME") == "admin" && server.arg("PASSWORD") == "esp32") {
      server.sendHeader("Location", "/");
      server.sendHeader("Cache-Control", "no-cache");
      server.sendHeader("Set-Cookie", "ESPSESSIONID=1");
      server.send(301);
      Serial.println("  Login Successful");
      return;
    }
    msg = "Wrong username/password! Try again.";
    Serial.println("  Login Failed");
  }
  String content = "<html><body>"
    "To log in, please use : admin/esp32<br>"
    "<form action='/login' method='POST'>"
    "User: <input type='text' name='USERNAME' placeholder='user name'><br>"
    "Password: <input type='password' name='PASSWORD' placeholder='password'><br>"
    "<input type='submit' name='SUBMIT' value='Submit'>"
    "</form>" + msg + "<br><br>"
    "You can also go <a href='/inline'>here</a>"
    "</body></html>";
  server.send(200, "text/html", content);
}

void handleRoot() {
  Serial.println("Enter handleRoot");
  String header;
  if (!is_authenticated()) {
    Serial.println("  Not authenticated. Redirecting.");
    server.sendHeader("Location", "/login");
    server.sendHeader("Cache-Control", "no-cache");
    server.send(301);
    return;
  }
  Serial.println("  Authenticated. Presenting data.");
  String content = "<html><body>"
    "<H2>Hello!</H2>"
    "You successfully connected to ESP32!<br>"
    "Access the data page <a href='/data'>here</a>.<br>";
  if (server.hasHeader("User-Agent")) {
    content += "the user agent is : " + server.header("User-Agent") + "<br>";
  }
  content += "<br>You can access this page until you <a href='/login?DISCONNECT=YES'>disconnect</a></body></html>";
  server.send(200, "text/html", content);
}

void handleDataPage() {
  Serial.println("Enter handleDataPage");
  String header;
  String gear = "P";
  int16_t speed = 0;
  int8_t battery = 100;
  if (!is_authenticated()) {
    Serial.println("  Not authenticated. Redirecting.");
    server.sendHeader("Location", "/login");
    server.sendHeader("Cache-Control", "no-cache");
    server.send(301);
    return;
  }
  Serial.println("  Authenticated. Presenting data.");
  if (server.hasArg("GEAR")) {
    gear = server.arg("GEAR");
    showGear(gear);
  }
  if (server.hasArg("SPEED")) {
    speed = server.arg("SPEED").toInt();
    showSpeed(speed);
  }
  if (server.hasArg("BATTERY")) {
    battery = server.arg("BATTERY").toInt();
    showBattery(battery);
  }
  String content = "<html><body>"
    "<H2>Data Entry Form</H2>\n"
    "<form action='/data' method='POST'>\n"
    "Gear: <input type='radio' name='GEAR' id='p' value='P'";
  content += (gear == "P" ? " checked/>" : "/>");
  content += "<label for='p'>Park</label>&emsp;"
    "<input type='radio' name='GEAR' id='n' value='N'";
  content += (gear == "N" ? " checked/>" : "/>");
  content += "<label for='n'>Neutral</label>&emsp;"
    "<input type='radio' name='GEAR' id='r' value='R'";
  content += (gear == "R" ? " checked/>" : "/>");
  content += "<label for='r'>Reverse</label>&emsp;"
    "<input type='radio' name='GEAR' id='d' value='D'";
  content += (gear == "D" ? " checked/>" : "/>");
  content += "<label for='d'>Drive</label><br>\n"
    "Speed: <input type='range' name='SPEED' min='0' max='150' value=" + String(speed) + "><br>"
    "Battery: <input type='range' name='BATTERY' min='0' max='100' value=" + String(battery) + "><br>"
    "<input type='submit' name='SUBMIT' value = 'Submit'>"
    "</form><br>";
  content += "<br>You can <a href='/login?DISCONNECT=YES'>disconnect</a>\n"
    "</body></html>";
  server.send(200, "text/html", content);
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguements: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup() {
  Serial.begin(115200);
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(BACKGROUND_COLOR);
  tft.setTextDatum(TC_DATUM);
  tft.invertDisplay(true);
  tft.setTextColor(TFT_BLUE, BACKGROUND_COLOR);
  tft.drawString("Sophia", tft.width()/2, 10, 4);
  tft.setTextColor(TFT_YELLOW, BACKGROUND_COLOR);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Connected");
  Serial.print("IP Address ");
  Serial.println(WiFi.localIP());
  tft.drawString(WiFi.localIP().toString(), tft.width()/2, 300, 4);

  server.on("/", handleRoot);
  server.on("/login", handleLogin);
  server.on("/data", handleDataPage);
  server.on("/inline", []() {
    server.send(200, "text/plain", "this works without need of authentication");
  });
  server.onNotFound(handleNotFound);

  const char* headerkeys[] = {"User-Agent", "Cookie"};
  size_t headerkeyssize = sizeof(headerkeys) / sizeof(char *);
  server.collectHeaders(headerkeys, headerkeyssize);
  
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
  delay(2);
}