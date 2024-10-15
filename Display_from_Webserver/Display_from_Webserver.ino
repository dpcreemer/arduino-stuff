#include <SPI.h>
#include <WiFi.h>
#include <WebServer.h>
#include <TFT_eSPI.h>

const char* SSID = "CYD is Cool";
const char* PASSWORD = "cydRulez!";
const int16_t COLOR_BG = TFT_BLACK;

WebServer server(80);
TFT_eSPI tft = TFT_eSPI();
String header;
bool showSetupScreen = true;

void drawMessage(String msg, int x, int y, uint16_t color) {
  tft.setTextDatum(TL_DATUM);
  tft.setTextColor(color, COLOR_BG);
  tft.drawString(msg, x, y, 2);
}

void drawGuage(String label, int16_t value, int16_t max, int32_t x, int32_t y) {
  tft.setTextDatum(TC_DATUM);
  tft.fillCircle(x, y, 58, COLOR_BG);
  tft.drawArc(x, y, 53, 51, 60, 300, TFT_LIGHTGREY, COLOR_BG);
  tft.drawArc(x, y, 54, 50, 60, 60 + (240 * value / max), TFT_GREEN, COLOR_BG);
  tft.drawString(String(value), x, y-20, 6);
  tft.drawString(label, x, y + 30, 4);
}

void checkForArgs() {
  if (server.args() > 0) tft.fillScreen(COLOR_BG);
  for (int i = 0; i < server.args(); i++){
    if (server.argName(i) == "speed") drawGuage("Speed", server.arg(i).toInt(), 150, 60, 120);
    else if (server.argName(i) == "charge") drawGuage("Charge", server.arg(i).toInt(), 100, 180, 120);
    else drawMessage(server.argName(i) + ": " + server.arg(i), 5, 5 + 25 * i, TFT_WHITE);
  }
}

void handleRoot() {
  Serial.println("Enter handleRoot");
  if (showSetupScreen) {
    tft.fillScreen(COLOR_BG);
    drawMessage("connection made", 5, 5, TFT_WHITE);
  }
  checkForArgs();
  String content = "<html><body>"
    "<H2>Hello!</H2>"
    "You have connected to CYD!<BR>"
    "</body></html>";
  server.send(200, "text/html", content);
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) 
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  server.send(404, "text/plain", message);
}

void setup() {
  Serial.begin(115200);
  tft.init();
  tft.setRotation(0);
  tft.invertDisplay(true);

  tft.setTextDatum(TL_DATUM);
  tft.setTextColor(TFT_GREEN);
  tft.fillScreen(COLOR_BG);

  Serial.println("Starting soft AP.");
  WiFi.softAP(SSID, PASSWORD);
  IPAddress IP = WiFi.softAPIP();
  Serial.println("IP: " + IP.toString());
  drawMessage("Soft AP Started", 5, 5, TFT_WHITE);
  drawMessage("Connect to SSID:", 5, 45, TFT_WHITE);
  drawMessage(SSID, 25, 70, TFT_BLUE);
  drawMessage("Password:", 5, 95, TFT_WHITE);
  drawMessage(PASSWORD, 25, 120, TFT_BLUE);
  drawMessage("Connect to url:", 5, 145, TFT_WHITE);
  Serial.println("Starting Web Server.");

  server.on("/", handleRoot);
  server.onNotFound(handleNotFound);

  const char* headerKeys[] = {"User-Agent", "Cookie"};
  size_t headerKeySize = sizeof(headerKeys) / sizeof(char *);
  server.collectHeaders(headerKeys, headerKeySize);

  server.begin();
  drawMessage("http://" + IP.toString() + "/", 25, 170, TFT_BLUE);
}

void loop() {
  server.handleClient();
  delay(2);
}
/*
void blah() {
  WiFiClient client = server.available();
  if (client) {
    Serial.println("Client connected");
    tft.fillScreen(COLOR_BG);
    drawMessage("Client Connected", 5, 5, TFT_WHITE);
    String currentLine = "";
    while (client.connected()) {
      char c = client.read();
      Serial.write(c);
      header += c;
      if (c != '\n' && c != '\r') {
        currentLine += c;
      } else if (c == '\n') {
        if (currentLine.length() > 0) {
          currentLine = "";
        } else {
          client.println("HTTP/1.1 200 OK");
          client.println("Content-type:text/html");
          client.println("Connection: close\n");

          client.println("<!DOCTYPE html><html>");
          client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
          client.println("<link rel=\"icon\" href=\"data:,\">");
          client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
          client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
          client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
          client.println(".button2 {background-color: #555555;}</style></head>");
          client.println("<body><h1>ESP32 Web Server</h1>");
          client.println("</body></html>");
          client.println();
          break; 
        }
      }
    }
    header = "";
    client.stop();
    Serial.println("Client Disconnected.");
    drawMessage("Client disconnected", 5, 40, TFT_WHITE);
  }
} */