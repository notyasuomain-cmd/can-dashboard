#include <SPI.h>
#include "Config.h"
#include "Data.h"
#include "Widgets.h"
#include "Pages.h"

Adafruit_ILI9341 tft(TFT_CS, TFT_DC, TFT_RST);
float smoothing = DEFAULT_SMOOTHING;

// ===== Gauges: add new gauges here =====
const float RPM_MAX = 6000, SPEED_MAX = 180;

ArcGauge rpmGauge  (W / 4,     H * 3 / 5, 70, {"RPM",   "%4.0f rpm",  0, RPM_MAX,   5000, RPM_MAX,   ILI9341_RED},    &VehicleData::rpm);
ArcGauge speedGauge(W * 3 / 4, H * 3 / 5, 70, {"Speed", "%3.0f km/h", 0, SPEED_MAX, 140,  SPEED_MAX, ILI9341_GREEN},  &VehicleData::speed);

ArcGauge tempGauge (W / 4,     110, 60, {"Temp", "%3.0f C", 40, 120, 105, 120,  ILI9341_ORANGE}, &VehicleData::temp);
ArcGauge battGauge (W * 3 / 4, 110, 60, {"Batt", "%4.1f V", 10, 15,  10,  11.8, ILI9341_YELLOW}, &VehicleData::battery);
BarGauge fuelBar   (66, 202, 180, 16,   {"Fuel", "%3.0f%%", 0,  100, 0,   15,   ILI9341_GREEN},  &VehicleData::fuel);

// ===== Pages: add new pages here =====
Widget* mainWidgets[]   = { &rpmGauge, &speedGauge };
Widget* engineWidgets[] = { &tempGauge, &battGauge, &fuelBar };

Page pages[] = {
  Page("MAIN",   mainWidgets,   COUNT(mainWidgets)),
  Page("ENGINE", engineWidgets, COUNT(engineWidgets)),
};
const int PAGE_COUNT = COUNT(pages);
int currentPage = 0;

// ===== Navigation =====
void drawHeader() {
  tft.setTextSize(1);
  tft.setTextColor(THEME.text, THEME.bg);
  tft.setCursor(6, 6);
  tft.print(pages[currentPage].title);

  for (int i = 0; i < PAGE_COUNT; i++) {     // page indicator dots, top right
    int x = W - 12 - (PAGE_COUNT - 1 - i) * 12;
    if (i == currentPage) tft.fillCircle(x, 9, 3, THEME.text);
    else                  tft.drawCircle(x, 9, 3, THEME.text);
  }
}

void showPage(int i) {
  pages[currentPage].leave();                // free the previous page's buffers
  currentPage = i;
  pages[i].enter();
  drawHeader();
}

bool buttonPressed() {                         // press edge, debounced
  static bool lastState = HIGH;
  static unsigned long lastChange = 0;
  bool state = digitalRead(BTN_PIN);
  if (state != lastState && millis() - lastChange > 50) {
    lastChange = millis();
    lastState = state;
    return state == LOW;
  }
  return false;
}

void startupSweep() {                          // needles sweep up and back at power-on
  smoothing = 1.0;
  for (int i = 0; i <= 50; i++) {
    float k = i <= 25 ? i / 25.0 : (50 - i) / 25.0;
    pages[0].update({ k * RPM_MAX, k * SPEED_MAX, 0, 0, 0 });
    delay(20);
  }
  smoothing = DEFAULT_SMOOTHING;
}

// ===== Main =====
unsigned long lastFrame = 0;

void setup() {
  Serial.begin(115200);
  pinMode(BTN_PIN, INPUT_PULLUP);
  tft.begin(SPI_FREQ);
  tft.setRotation(1);
  showPage(0);
  startupSweep();
}

void loop() {
  if (buttonPressed()) showPage((currentPage + 1) % PAGE_COUNT);

  if (millis() - lastFrame >= FRAME_MS) {
    lastFrame = millis();
    pages[currentPage].update(readData());
  }
}