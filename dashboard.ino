#include <TFT_eSPI.h>
#include <SPI.h>

TFT_eSPI tft = TFT_eSPI();

// Gauge középpontok
const int rpmX = 120;
const int speedX = 360;
const int centerY = 160;
const int radius = 100;

int lastRpmAngle = -999;
int lastSpeedAngle = -999;

void drawGaugeBase(int cx, const String& label, int redStart, int redEnd, int maxValue) {
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextDatum(MC_DATUM);

  // Skála vonalak
  for (int i = 180; i <= 360; i += 15) {
    float angleRad = radians(i);
    int x1 = cx + cos(angleRad) * (radius - 10);
    int y1 = centerY + sin(angleRad) * (radius - 10);
    int x2 = cx + cos(angleRad) * radius;
    int y2 = centerY + sin(angleRad) * radius;
    tft.drawLine(x1, y1, x2, y2, TFT_WHITE);
  }

  // Piros zóna (piros ív)
  int angleStart = map(redStart, 0, maxValue, 180, 360);
  int angleEnd   = map(redEnd,   0, maxValue, 180, 360);

  for (int a = angleStart; a <= angleEnd; a++) {
    float rad = radians(a);
    int x = cx + cos(rad) * radius;
    int y = centerY + sin(rad) * radius;
    tft.drawPixel(x, y, TFT_RED);
  }

  // Felirat
  tft.drawCentreString(label, cx, centerY + radius + 10, 2);
}

void drawNeedle(int angle, int cx, int& lastAngle, uint16_t color) {
  // Töröljük az előző mutatót
  if (lastAngle != -999) {
    float oldRad = radians(lastAngle);
    int oldX = cx + cos(oldRad) * (radius - 30);
    int oldY = centerY + sin(oldRad) * (radius - 30);
    tft.drawLine(cx, centerY, oldX, oldY, TFT_BLACK);
  }

  float rad = radians(angle);
  int x = cx + cos(rad) * (radius - 30);
  int y = centerY + sin(rad) * (radius - 30);
  tft.drawLine(cx, centerY, x, y, color);

  lastAngle = angle;
}

void drawValues(int rpm, int speed) {
  tft.fillRect(0, 260, 480, 40, TFT_BLACK); // Tisztít
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.setTextDatum(TL_DATUM);
  tft.drawString("RPM: " + String(rpm), 40, 270, 2);
  tft.drawString("Speed: " + String(speed) + " km/h", 260, 270, 2);
}

void setup() {
  tft.init();
  tft.setRotation(1);  // Fekvő mód
  tft.fillScreen(TFT_BLACK);

  drawGaugeBase(rpmX, "RPM", 5000, 6000, 6000);      // 5000-6000 RPM piros
  drawGaugeBase(speedX, "km/h", 140, 180, 180);      // 140-180 km/h piros
}

void loop() {
  for (int i = 0; i <= 100; i++) {
    int rpm = map(i, 0, 100, 0, 6000);
    int speed = map(i, 0, 100, 0, 180);

    // Mutató szög: 180° (bal) → 360° (jobb), alap 270° (le)
    int rpmAngle = map(rpm, 0, 6000, 180, 360);
    int speedAngle = map(speed, 0, 180, 180, 360);

    drawNeedle(rpmAngle, rpmX, lastRpmAngle, TFT_RED);
    drawNeedle(speedAngle, speedX, lastSpeedAngle, TFT_GREEN);

    drawValues(rpm, speed);
    delay(40);
  }

  delay(300);
}
