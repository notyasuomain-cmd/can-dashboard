#pragma once
#include "Config.h"
#include "Data.h"

inline bool blinkOn() { return (millis() / 300) % 2 == 0; }

// Centered text on any drawing surface (display or off-screen canvas)
inline void drawCentered(Adafruit_GFX& g, const char* text, int cx, int cy, uint16_t color) {
  int16_t x1, y1;
  uint16_t w, h;
  g.setTextSize(2);
  g.setTextColor(color);
  g.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  g.setCursor(cx - w / 2, cy - h / 2);
  g.print(text);
}

struct GaugeCfg {
  const char* label;
  const char* fmt;        // value format, must be fixed width, e.g. "%4.0f rpm"
  float minV, maxV;
  float redFrom, redTo;   // warning zone, can be at the low end too (e.g. fuel)
  uint16_t color;         // needle / bar color
};

// Base class: every gauge renders into its own off-screen buffer and pushes
// the finished image to the display in one go -> no flicker.
class Widget {
public:
  Widget(int x, int y, int w, int h, float VehicleData::* f)
    : x(x), y(y), w(w), h(h), field(f) {}
  virtual ~Widget() { end(); }

  void begin() {                       // called when the page is shown
    if (!canvas) canvas = new GFXcanvas16(w, h);
    if (!canvas->getBuffer()) Serial.println("Not enough memory for gauge buffer!");
    resetState();
    dirty = true;
  }

  void end() {                         // called when the page is left: free the buffer
    delete canvas;
    canvas = nullptr;
  }

  void update(const VehicleData& d) {
    shown += (d.*field - shown) * smoothing;
    if (!canvas || !canvas->getBuffer()) return;

    if (prepare() || dirty) {          // redraw only on visible change
      canvas->fillScreen(THEME.bg);
      render(*canvas);
      tft.drawRGBBitmap(x, y, canvas->getBuffer(), w, h);
      dirty = false;
    }
  }

protected:
  virtual void resetState() = 0;
  virtual bool prepare() = 0;                    // compute what to draw; true if it changed
  virtual void render(GFXcanvas16& c) = 0;       // draw into the buffer, local coordinates

  int x, y, w, h;                                // position and size on the display
  float VehicleData::* field;                    // which value to show, e.g. &VehicleData::rpm
  GFXcanvas16* canvas = nullptr;
  bool dirty = true;
  float shown = 0;                               // smoothed, currently displayed value
};

class ArcGauge : public Widget {
public:
  ArcGauge(int cx, int cy, int r, GaugeCfg cfg, float VehicleData::* f)
    : Widget(cx - r - 2, cy - r - 2, 2 * r + 5, r + 55, f), r(r), cfg(cfg) {}

protected:
  void resetState() override {
    shown = cfg.minV;     // needle sweeps up from zero when the page is shown
    lastAngle = -999;
    lastText[0] = '\0';
  }

  bool prepare() override {
    angle = toAngle(shown);
    snprintf(text, sizeof(text), cfg.fmt, shown);
    bool alarm = shown >= cfg.redFrom && shown <= cfg.redTo;
    textColor = alarm && blinkOn() ? THEME.red : THEME.value;

    bool changed = angle != lastAngle || strcmp(text, lastText) != 0 || textColor != lastColor;
    lastAngle = angle;
    strcpy(lastText, text);
    lastColor = textColor;
    return changed;
  }

  void render(GFXcanvas16& c) override {
    int lcx = r + 2, lcy = r + 2;               // center inside the buffer

    // Warning zone
    for (float a = toAngle(cfg.redFrom); a <= toAngle(cfg.redTo); a += 0.5) {
      c.drawLine(px(lcx, a, r - 8), py(lcy, a, r - 8), px(lcx, a, r), py(lcy, a, r), THEME.red);
    }
    // Scale ticks
    for (int a = 180; a <= 360; a += 15) {
      c.drawLine(px(lcx, a, r - 10), py(lcy, a, r - 10), px(lcx, a, r), py(lcy, a, r), THEME.scale);
    }

    drawCentered(c, cfg.label, lcx, lcy + 18, THEME.text);
    drawCentered(c, text, lcx, lcy + 42, textColor);

    // Triangular needle
    float rad  = radians(angle);
    float perp = rad + HALF_PI;
    int len = r - 18;
    c.fillTriangle(lcx + cos(rad) * len,  lcy + sin(rad) * len,
                   lcx + cos(perp) * 3,   lcy + sin(perp) * 3,
                   lcx - cos(perp) * 3,   lcy - sin(perp) * 3, cfg.color);
    c.fillCircle(lcx, lcy, 5, THEME.text);      // hub
  }

private:
  int r;
  GaugeCfg cfg;
  int angle = 0, lastAngle = -999;
  char text[16] = "", lastText[16] = "";
  uint16_t textColor = 0, lastColor = 0;

  // Screen angles: 180° = left, 270° = up, 360° = right
  float toAngle(float v) {
    v = constrain(v, cfg.minV, cfg.maxV);
    return 180.0 + 180.0 * (v - cfg.minV) / (cfg.maxV - cfg.minV);
  }
  int px(int cx, float deg, int rr) { return cx + cos(radians(deg)) * rr; }
  int py(int cy, float deg, int rr) { return cy + sin(radians(deg)) * rr; }
};

class BarGauge : public Widget {
public:
  // bx, by, bw, bh: the bar itself; the buffer is wider to fit the label and value
  BarGauge(int bx, int by, int bw, int bh, GaugeCfg cfg, float VehicleData::* f)
    : Widget(bx - 62, by - 4, bw + 124, bh + 8, f), bw(bw), bh(bh), cfg(cfg) {}

protected:
  void resetState() override {
    shown = cfg.minV;
    lastFill = -1;
    lastText[0] = '\0';
  }

  bool prepare() override {
    float v = constrain(shown, cfg.minV, cfg.maxV);
    fill = (bw - 4) * (v - cfg.minV) / (cfg.maxV - cfg.minV);
    bool alarm = shown >= cfg.redFrom && shown <= cfg.redTo;
    barColor  = alarm ? THEME.red : cfg.color;
    textColor = alarm && blinkOn() ? THEME.red : THEME.value;
    snprintf(text, sizeof(text), cfg.fmt, shown);

    bool changed = fill != lastFill || barColor != lastBar ||
                   textColor != lastColor || strcmp(text, lastText) != 0;
    lastFill = fill;
    lastBar = barColor;
    lastColor = textColor;
    strcpy(lastText, text);
    return changed;
  }

  void render(GFXcanvas16& c) override {
    const int bx = 62, by = 4;                  // bar position inside the buffer
    c.drawRect(bx, by, bw, bh, THEME.scale);
    if (fill > 0) c.fillRect(bx + 2, by + 2, fill, bh - 4, barColor);

    drawCentered(c, cfg.label, bx - 34, by + bh / 2, THEME.text);
    drawCentered(c, text, bx + bw + 34, by + bh / 2, textColor);
  }

private:
  int bw, bh;
  GaugeCfg cfg;
  int fill = 0, lastFill = -1;
  uint16_t barColor = 0, lastBar = 0, textColor = 0, lastColor = 0;
  char text[16] = "", lastText[16] = "";
};