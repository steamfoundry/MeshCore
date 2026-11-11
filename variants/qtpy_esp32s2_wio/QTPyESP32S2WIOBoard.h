#pragma once

#include <Arduino.h>
#include <helpers/ESP32Board.h>


class QTPyESP32S2WIOBoard : public ESP32Board {
public:
  QTPyESP32S2WIOBoard() {}

  const char* getManufacturerName() const override {
    return "Adafruit QTPy ESP32-S2 + Seeed Wio-SX1262";
  }
}
