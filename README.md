# Arduino TFT Dashboard Demo

This project demonstrates a simple animated dashboard using a TFT display and the TFT_eSPI library. It simulates two analog gauges: one for RPM (revolutions per minute) and one for speed (km/h), complete with moving needles and red warning zones.

## Features
- Two circular gauges: RPM and speed
- Animated needles that sweep from minimum to maximum values
- Red zone indicators for high RPM and speed
- Real-time value display for RPM and speed
- Clean and modern graphical interface

## Hardware Requirements
- Arduino-compatible board (e.g., ESP32, ESP8266, Arduino UNO with enough memory)
- TFT display compatible with the TFT_eSPI library (e.g., ILI9341, ST7735, etc.)
- Proper wiring between the Arduino and the TFT display

## Libraries Used
- [TFT_eSPI](https://github.com/Bodmer/TFT_eSPI)
- SPI (usually included with Arduino IDE)

## Usage
1. Connect your TFT display to your Arduino board according to the TFT_eSPI library's wiring instructions.
2. Install the TFT_eSPI library via the Arduino Library Manager or from GitHub.
3. Copy the contents of `dashboard.ino` to your Arduino IDE.
4. Adjust the display driver and pin settings in the `User_Setup.h` file of the TFT_eSPI library if needed.
5. Upload the sketch to your board.
6. The display will show two animated gauges for RPM and speed.

## Customization
- You can change the maximum values, red zone ranges, and gauge positions by editing the constants at the top of `dashboard.ino`.
- The animation speed can be adjusted by changing the `delay()` values in the `loop()` function.

## WIP / Roadmap
This project is under active development. Planned features include:
- Support for multiple gauges (e.g., temperature, fuel, battery, etc.)
- Multiple dashboard pages with navigation
- Improved UI and customization options
- Additional animations and effects
- Code refactoring for modularity and scalability

Stay tuned for updates!

## License
This project is provided for educational and demonstration purposes. Feel free to modify and use it in your own projects.
