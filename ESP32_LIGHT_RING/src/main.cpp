#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <ESPmDNS.h>
#include <FastLED.h>
#include "config.h"

// Access Point credentials
const char *AP_SSID = "ESP32-LightRing";
const char *AP_PASSWORD = "lightring"; // min 8 characters, set to "" for open network

// mDNS hostname - reachable as "light-ring.local" when connected to the AP
const char *MDNS_HOSTNAME = "light-ring";

CRGB leds[NUM_LEDS];
bool ledsOn = false;
CRGB currentColor = CRGB::White;

WebServer server(80);
DNSServer dnsServer;

void applyLeds()
{
  if (ledsOn)
  {
    fill_solid(leds, NUM_LEDS, currentColor);
  }
  else
  {
    fill_solid(leds, NUM_LEDS, CRGB::Black);
  }
  FastLED.show();
}

void handleRoot()
{
  String onClass  = ledsOn ? "btn btn-on active" : "btn btn-on";
  String offClass = ledsOn ? "btn btn-off" : "btn btn-off active";

  String html = R"rawhtml(<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ESP32 Light Ring</title>
  <style>
    * { box-sizing: border-box; margin: 0; padding: 0; }
    body {
      font-family: Arial, sans-serif;
      background: #1a1a2e;
      color: #eee;
      display: flex;
      flex-direction: column;
      align-items: center;
      padding: 40px 20px;
      min-height: 100vh;
    }
    h1 { font-size: 2rem; margin-bottom: 8px; letter-spacing: 2px; }
    p.subtitle { color: #aaa; margin-bottom: 32px; }
    .card {
      background: #16213e;
      border-radius: 16px;
      padding: 32px;
      max-width: 420px;
      width: 100%;
      box-shadow: 0 8px 32px rgba(0,0,0,0.4);
    }
    .section-title { font-size: 0.85rem; text-transform: uppercase; letter-spacing: 1px; color: #888; margin-bottom: 12px; }
    .power-row { display: flex; gap: 12px; margin-bottom: 32px; }
    .btn {
      flex: 1;
      padding: 14px;
      border: 2px solid #333;
      border-radius: 10px;
      background: #0f3460;
      color: #eee;
      font-size: 1rem;
      font-weight: bold;
      cursor: pointer;
      text-decoration: none;
      text-align: center;
      transition: background 0.2s, border-color 0.2s;
    }
    .btn:hover { background: #1a4a80; }
    .btn.active { background: #e94560; border-color: #e94560; color: #fff; }
    .btn-on.active { background: #27ae60; border-color: #27ae60; }
    .color-grid {
      display: grid;
      grid-template-columns: repeat(4, 1fr);
      gap: 10px;
    }
    .color-btn {
      width: 100%;
      aspect-ratio: 1;
      border: 3px solid transparent;
      border-radius: 50%;
      cursor: pointer;
      transition: transform 0.15s, border-color 0.15s;
    }
    .color-btn:hover { transform: scale(1.12); }
    .color-btn.selected { border-color: #fff; transform: scale(1.12); }
  </style>
</head>
<body>
  <h1>&#128161; Light Ring</h1>
  <p class="subtitle">ESP32 WS2812B Controller</p>
  <div class="card">
    <div class="section-title">Power</div>
    <div class="power-row">
      <a href="/on"  class=")rawhtml" + onClass  + R"rawhtml(">ON</a>
      <a href="/off" class=")rawhtml" + offClass + R"rawhtml(">OFF</a>
    </div>
    <div class="section-title">Color</div>
    <div class="color-grid">
      <a href="/color?r=255&g=255&b=255" title="White"   style="background:#ffffff" class="color-btn)rawhtml" + (currentColor == CRGB(255,255,255) ? " selected" : "") + R"rawhtml("></a>
      <a href="/color?r=255&g=0&b=0"     title="Red"     style="background:#ff0000" class="color-btn)rawhtml" + (currentColor == CRGB(255,0,0)     ? " selected" : "") + R"rawhtml("></a>
      <a href="/color?r=0&g=255&b=0"     title="Green"   style="background:#00ff00" class="color-btn)rawhtml" + (currentColor == CRGB(0,255,0)     ? " selected" : "") + R"rawhtml("></a>
      <a href="/color?r=0&g=0&b=255"     title="Blue"    style="background:#0000ff" class="color-btn)rawhtml" + (currentColor == CRGB(0,0,255)     ? " selected" : "") + R"rawhtml("></a>
      <a href="/color?r=255&g=165&b=0"   title="Orange"  style="background:#ffa500" class="color-btn)rawhtml" + (currentColor == CRGB(255,165,0)   ? " selected" : "") + R"rawhtml("></a>
      <a href="/color?r=255&g=255&b=0"   title="Yellow"  style="background:#ffff00" class="color-btn)rawhtml" + (currentColor == CRGB(255,255,0)   ? " selected" : "") + R"rawhtml("></a>
      <a href="/color?r=0&g=255&b=255"   title="Cyan"    style="background:#00ffff" class="color-btn)rawhtml" + (currentColor == CRGB(0,255,255)   ? " selected" : "") + R"rawhtml("></a>
      <a href="/color?r=255&g=0&b=255"   title="Magenta" style="background:#ff00ff" class="color-btn)rawhtml" + (currentColor == CRGB(255,0,255)   ? " selected" : "") + R"rawhtml("></a>
    </div>
  </div>
</body>
</html>)rawhtml";

  server.send(200, "text/html", html);
}

void handleOn()
{
  ledsOn = true;
  applyLeds();
  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "");
}

void handleOff()
{
  ledsOn = false;
  applyLeds();
  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "");
}

void handleColor()
{
  if (!server.hasArg("r") || !server.hasArg("g") || !server.hasArg("b"))
  {
    server.send(400, "text/plain", "Missing r, g, or b parameter");
    return;
  }
  uint8_t r = (uint8_t)constrain(server.arg("r").toInt(), 0, 255);
  uint8_t g = (uint8_t)constrain(server.arg("g").toInt(), 0, 255);
  uint8_t b = (uint8_t)constrain(server.arg("b").toInt(), 0, 255);
  currentColor = CRGB(r, g, b);
  if (ledsOn)
  {
    applyLeds();
  }
  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "");
}

void handleNotFound()
{
  server.send(404, "text/plain", "Not found");
}

void setup()
{
  Serial.begin(115200);

  // Initialize LED ring
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(128);
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();

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
  if (MDNS.begin(MDNS_HOSTNAME))
  {
    MDNS.addService("http", "tcp", 80);
    Serial.print("mDNS started - open http://");
    Serial.print(MDNS_HOSTNAME);
    Serial.println(".local");
  }
  else
  {
    Serial.println("mDNS failed to start");
  }

  // Register routes
  server.on("/", handleRoot);
  server.on("/on", handleOn);
  server.on("/off", handleOff);
  server.on("/color", handleColor);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
  Serial.println("Connect to Wi-Fi \"" + String(AP_SSID) + "\" and open http://light.ring");
}

void loop()
{
  dnsServer.processNextRequest();
  server.handleClient();
}