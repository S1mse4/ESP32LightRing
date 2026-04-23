#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <ESPmDNS.h>

// Access Point credentials
const char *AP_SSID = "ESP32-LightRing";
const char *AP_PASSWORD = "lightring";  // min 8 characters, set to "" for open network

// mDNS hostname - reachable as "light-ring.local" when connected to the AP
const char *MDNS_HOSTNAME = "light-ring";

WebServer server(80);
DNSServer dnsServer;

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

  // Start DNS server - resolves every hostname (including "light.ring") to the AP IP
  // Port 53 is the standard DNS port; TTL 0 means no caching
  dnsServer.start(53, "*", ip);
  Serial.println("DNS server started (wildcard -> " + ip.toString() + ")");

  // Start mDNS so the device is also reachable as "light-ring.local"
  if (MDNS.begin(MDNS_HOSTNAME)) {
    MDNS.addService("http", "tcp", 80);
    Serial.print("mDNS started - open http://");
    Serial.print(MDNS_HOSTNAME);
    Serial.println(".local");
  } else {
    Serial.println("mDNS failed to start");
  }

  // Register routes
  server.on("/", handleRoot);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
  Serial.println("Connect to Wi-Fi \"" + String(AP_SSID) + "\" and open http://light.ring");
}

void loop() {
  dnsServer.processNextRequest();
  server.handleClient();
}