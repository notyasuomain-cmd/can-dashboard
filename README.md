# ESP32 TFT Dashboard

Animated, flicker-free car-style dashboard for the ESP32 and an ILI9341 TFT display.
Multiple pages, smooth needles, warning zones and a startup needle sweep.
Runs on real hardware or entirely in the browser with Wokwi.

**[Try it in Wokwi](https://wokwi.com/projects/478960465737987073)** - no hardware needed.

## Features

- Two pages switched with a push button: **Main** (RPM, speed) and **Engine** (coolant temperature, battery voltage, fuel level)
- Flicker-free rendering: each gauge is drawn into an off-screen buffer (`GFXcanvas16`) and pushed to the display in one go
- Smooth needle movement and a startup needle sweep
- Warning zones at the high or low end, with a blinking value when entered
- Data layer separated from the UI: replace a single function to show real data
- New gauges and pages can be added with one line of configuration

## Hardware

| Part | Notes |
|---|---|
| ESP32 DevKit (classic ESP32) | about 75 KB free RAM is needed for the gauge buffers |
| ILI9341 TFT, 320×240, SPI | 2.4" or 2.8" modules work |
| Push button | page navigation |

### Wiring

| ILI9341 | ESP32 |
|---|---|
| VCC, LED | 3V3 |
| GND | GND |
| SCK | GPIO18 |
| MOSI | GPIO23 |
| MISO | GPIO19 |
| CS | GPIO27 |
| D/C | GPIO26 |
| RST | GPIO25 |

Push button: one leg to GPIO14, the other to GND (the internal pull-up is used).

## Getting started

### Arduino IDE

1. Install the ESP32 board package (Boards Manager, "esp32" by Espressif).
2. Install **Adafruit ILI9341** and **Adafruit GFX Library** from the Library Manager.
3. Open `dashboard/dashboard.ino`, select your ESP32 board and upload.

### Wokwi

Open the link above, or create a new ESP32 project, copy `wokwi/diagram.json` and
`wokwi/libraries.txt`, and add the files from `dashboard/`
(in Wokwi the main file is called `sketch.ino`).

## Project structure

| File | Purpose |
|---|---|
| `dashboard.ino` | gauge and page configuration, navigation, main loop |
| `Config.h` | pins, display settings, theme, timing |
| `Data.h` | `VehicleData` struct and the data source |
| `Widgets.h` | `ArcGauge` and `BarGauge` |
| `Pages.h` | page handling |

## Customization

Add a gauge by adding a field to `VehicleData`, filling it in `readData()`,
and declaring the gauge in `dashboard.ino`:

```cpp
ArcGauge oilGauge(W / 4, 110, 60, {"Oil", "%3.0f C", 40, 150, 130, 150, ILI9341_ORANGE}, &VehicleData::oil);
```

Then put it into one of the page lists. Colors are set in `THEME` in `Config.h`.

## Design notes

The first version used TFT_eSPI. It was replaced with Adafruit_ILI9341 because
TFT_eSPI's pin configuration has to live inside the library folder (it cannot be set
from the sketch, which makes online simulation awkward), and because it crashed on
newer ESP32 Arduino cores. Adafruit_GFX is slower, but off-screen buffering keeps the
animation smooth and flicker-free.

## Roadmap

- [x] Multiple gauges (temperature, fuel, battery)
- [x] Multiple pages with navigation
- [x] Flicker-free rendering
- [x] Smooth needles, startup sweep, blinking warnings
- [x] Modular code structure
- [ ] Real data source (OBD-II, CAN bus or sensors)
- [ ] Settings page with persistent storage
- [ ] Touch screen navigation
- [ ] Peak value markers, custom fonts, buzzer

## License

MIT, see [LICENSE](LICENSE).