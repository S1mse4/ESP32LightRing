#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

const char* ssid     = "LED Ring";
const char* password = "light";

WebServer server(80);

void handleRoot() {
  server.send(200, "text/html", "<html><body><h1>Hello World</h1></body></html>");
}

void setup() {
  Serial.begin(115200);

  WiFi.softAP(ssid, password);

  IPAddress ip = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(ip);

  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
