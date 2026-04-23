#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

// Access Point credentials
const char *AP_SSID = "ESP32-LightRing";
const char *AP_PASSWORD = "lightring";  // min 8 characters, set to "" for open network

WebServer server(80);

void handleRoot() {
  server.send(200, "text/html", "<html><body><h1>Hello World</h1></body></html>");
}

void handleNotFound() {
  server.send(404, "text/plain", "Not found");
}

void setup() {
  Serial.begin(115200);

  // Start Access Point
  WiFi.softAP(AP_SSID, AP_PASSWORD);
  IPAddress ip = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(ip);

  // Register routes
  server.on("/", handleRoot);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}