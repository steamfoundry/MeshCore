#pragma once

#include <Arduino.h>
#include <helpers/ESP32Board.h>

class QTPyESP32S2WIOBoard : public ESP32Board {
public:
  void begin() {
    ESP32Board::begin();
    pinMode(P_LORA_RF_SWITCH, OUTPUT);   //check this
    pinMode(P_LORA_TXEN, OUTPUT);
    pinMode(P_LORA_RXEN, OUTPUT);
    digitalWrite(P_LORA_RF_SWITCH, HIGH);
    setReceivePath();
  }

  void onBeforeTransmit() override {
    digitalWrite(P_LORA_RXEN, LOW);
    digitalWrite(P_LORA_TXEN, HIGH);
  }

  void onAfterTransmit() override {
    setReceivePath();
  }

  const char* getManufacturerName() const override {
    return "Adafruit QTPy ESP32-S2 + Seeed Wio-SX1262";
  }

private:
  static void setReceivePath() {
    digitalWrite(P_LORA_TXEN, LOW);
    digitalWrite(P_LORA_RXEN, HIGH);
  }
};
