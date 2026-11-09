#include <Arduino.h>
#include <SPI.h>
#include <helpers/radiolib/RadioLibWrappers.h>
#include "target.h"

QTPyESP32S2WIOBoard board;
static SPIClass radio_spi(FSPI);
RADIO_CLASS radio = new Module(P_LORA_NSS, P_LORA_DIO_1, P_LORA_RESET, P_LORA_BUSY, radio_spi);
WRAPPER_CLASS radio_driver(radio, board);
ESP32RTCClock fallback_clock;
AutoDiscoverRTCClock rtc_clock(fallback_clock);
EnvironmentSensorManager sensors;

bool radio_init() {
  fallback_clock.begin();
  rtc_clock.begin(Wire);
  radio_spi.begin(P_LORA_SCLK, P_LORA_MISO, P_LORA_MOSI, P_LORA_NSS);
  if (!radio.std_init(&radio_spi)) return false;
  radio_driver.begin();
  return true;
}

mesh::LocalIdentity radio_new_identity() {
  RadioNoiseListener rng(radio);
  return mesh::LocalIdentity(&rng);
}
