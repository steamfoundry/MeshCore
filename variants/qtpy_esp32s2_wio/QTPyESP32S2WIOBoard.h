#pragma once

#include <Arduino.h>
#include <helpers/ESP32Board.h>


class QTPyESP32S2WIOBoard : public ESP32Board {
public:
  QTPyESP32S2WIOBoard() {}

  const char* getManufacturerName() const override {
    return "Adafruit QTPy ESP32-S2 + Seeed Wio-SX1262";
  }

  void beginIndicator() {
    pinMode(NEOPIXEL_POWER, OUTPUT);
    digitalWrite(NEOPIXEL_POWER, HIGH);
    delay(10);
    setIndicatorColor(0, 32, 0);
  }

  void setIndicatorColor(uint8_t red, uint8_t green, uint8_t blue) {
    neopixelWrite(NEOPIXEL_LED, red, green, blue);
  }
};
