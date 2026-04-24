#pragma once
#include <Arduino.h>

struct VehicleData {
  float rpm, speed, temp, fuel, battery;
};

// Simulated values. Replace the body of this function with real
// sensor / CAN / OBD-II reading - the UI code does not need to change.
inline VehicleData readData() {
  float t = millis() / 1000.0;
  VehicleData d;
  d.rpm     = 3000 + 3000 * sin(t * 0.8);   // 0..6000
  d.speed   = 90 + 90 * sin(t * 0.5);       // 0..180 km/h
  d.temp    = 85 + 25 * sin(t * 0.1);       // 60..110 °C
  d.fuel    = 50 + 50 * cos(t * 0.05);      // 0..100 %
  d.battery = 12.6 + 1.6 * sin(t * 0.3);    // 11..14.2 V
  return d;
}