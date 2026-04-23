#pragma once
#include <Adafruit_NeoPixel.h>

// Hardware configuration for CJMCU-28128-12 (12x WS2812B LED ring)
#define NUM_LEDS       12                        // Number of LEDs on the ring
#define LED_PIN         4
#define NEO_PIXEL_TYPE (NEO_GRB + NEO_KHZ800)   // WS2812B, GRB color order
#define BRIGHTNESS     50                        // 0-255; keep low to avoid excessive brightness