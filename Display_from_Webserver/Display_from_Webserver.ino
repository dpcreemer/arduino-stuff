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
  tft.setTextColor(color, TFT_BLACK);
  tft.drawString(msg, x, y, 2);
}

void handleRoot() {
  Serial.println("Enter handleRoot");
  if (showSetupScreen) tft.fillScreen(COLOR_BG);
  if (server.hasArg("dog")) 
    drawMessage("dog: " + server.arg("dog"), 5, 5, TFT_WHITE);
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