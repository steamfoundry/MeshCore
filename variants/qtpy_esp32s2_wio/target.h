#pragma once

#define RADIOLIB_STATIC_ONLY 1
#include <Arduino.h>
#include <Mesh.h>
#include <helpers/radiolib/RadioLibWrappers.h>
#include <helpers/ESP32Board.h>
#include <helpers/AutoDiscoverRTCClock.h>
#include <helpers/sensors/EnvironmentSensorManager.h>
#include <helpers/radiolib/CustomSX1262.h>
#include <helpers/radiolib/CustomSX1262Wrapper.h>
#include "QTPyESP32S2WIOBoard.h"


extern QTPyESP32S2WIOBoard board;
extern WRAPPER_CLASS radio_driver;
extern AutoDiscoverRTCClock rtc_clock;
extern EnvironmentSensorManager sensors;

bool radio_init();
void install_crash_gdbstub();
mesh::LocalIdentity radio_new_identity();
