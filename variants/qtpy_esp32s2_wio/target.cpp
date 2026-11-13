#include <Arduino.h>
#include <SPI.h>
#include <helpers/radiolib/RadioLibWrappers.h>
#include <esp_gdbstub.h>
#include <xtensa/config/core.h>
#include <xtensa/corebits.h>
#include <xtensa/xtensa_api.h>
#include "target.h"

QTPyESP32S2WIOBoard board;

#if defined(P_LORA_SCLK)
  static SPIClass spi;
  RADIO_CLASS radio = new Module(P_LORA_NSS, P_LORA_DIO_1, P_LORA_RESET, P_LORA_BUSY, spi);
#else
  RADIO_CLASS radio = new Module(P_LORA_NSS, P_LORA_DIO_1, P_LORA_RESET, P_LORA_BUSY);
#endif

WRAPPER_CLASS radio_driver(radio, board);

ESP32RTCClock fallback_clock;
AutoDiscoverRTCClock rtc_clock(fallback_clock);
EnvironmentSensorManager sensors;

static void gdbstub_exception_handler(XtExcFrame* frame) {
  esp_gdbstub_panic_handler(frame);
}

void install_crash_gdbstub() {
  for (int cause = 0; cause < XCHAL_EXCCAUSE_NUM; ++cause) {
    if (cause != EXCCAUSE_LEVEL1_INTERRUPT) {
      xt_set_exception_handler(cause, gdbstub_exception_handler);
    }
  }
}


bool radio_init() {
  if (!Serial) {
    Serial.begin(115200);
    delay(1000);
  }
  Serial.println("starting radio_init");
  fallback_clock.begin();
  pinMode(18, INPUT); /*PIN_USER_BTN*/

 ///*Use the Stemma QT port for I2C*/
  //.begin(PIN_BOARD_SDA, PIN_BOARD_SCL);
  //rtc_clock.begin(Wire1);

#if defined(P_LORA_SCLK)
  spi.begin(P_LORA_SCLK, P_LORA_MISO, P_LORA_MOSI);
  return radio.std_init(&spi);
#else
  return radio.std_init();
#endif
}

mesh::LocalIdentity radio_new_identity() {
  RadioNoiseListener rng(radio);
  return mesh::LocalIdentity(&rng);
}
