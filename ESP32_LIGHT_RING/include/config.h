#pragma once

// Hardware configuration for CJMCU-28128-12 (12x WS2812B LED ring)
#define NUM_LEDS    12  // Number of LEDs on the ring
#define LED_PIN      4
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define BRIGHTNESS  50  // 0-255; keep low to avoid excessive brightness