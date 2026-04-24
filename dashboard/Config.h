#pragma once
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

// ===== Pins =====
#define TFT_CS  27
#define TFT_DC  26
#define TFT_RST 25
#define BTN_PIN 14                     // push button to GND, internal pull-up

// ===== Display =====
const int W = 320, H = 240;            // ILI9341 in landscape mode
const uint32_t SPI_FREQ = 40000000;    // lower to 27000000 if a real display shows glitches

// ===== Timing & animation =====
const unsigned long FRAME_MS = 40;     // ~25 fps
const float DEFAULT_SMOOTHING = 0.25;  // 1.0 = instant, lower = softer needle movement

// ===== Theme =====
struct Theme { uint16_t bg, text, scale, red, value; };
const Theme THEME = { ILI9341_BLACK, ILI9341_WHITE, ILI9341_WHITE, ILI9341_RED, ILI9341_CYAN };

#define COUNT(a) (sizeof(a) / sizeof((a)[0]))

// Defined in dashboard.ino
extern Adafruit_ILI9341 tft;
extern float smoothing;