#pragma once
#include "Config.h"
#include "Data.h"
#include "Widgets.h"

// A page is a list of widgets. Only the visible page holds gauge buffers.
class Page {
public:
  Page(const char* title, Widget** widgets, int count)
    : title(title), widgets(widgets), count(count) {}

  void enter() {
    tft.fillScreen(THEME.bg);
    for (int i = 0; i < count; i++) widgets[i]->begin();
  }
  void leave() {
    for (int i = 0; i < count; i++) widgets[i]->end();
  }
  void update(const VehicleData& d) {
    for (int i = 0; i < count; i++) widgets[i]->update(d);
  }

  const char* title;
private:
  Widget** widgets;
  int count;
};