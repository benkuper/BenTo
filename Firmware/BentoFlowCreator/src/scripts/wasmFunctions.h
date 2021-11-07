#pragma once

#include <wasm3.h>
#include <m3_env.h>

m3ApiRawFunction(m3_arduino_millis);
m3ApiRawFunction(m3_arduino_delay);

// This maps pin modes from arduino_wasm_api.h
// to actual platform-specific values
uint8_t mapPinMode(uint8_t mode);
m3ApiRawFunction(m3_arduino_pinMode);
m3ApiRawFunction(m3_arduino_digitalWrite);
m3ApiRawFunction(m3_arduino_print);
m3ApiRawFunction(m3_arduino_getPinLED);
m3ApiRawFunction(m3_arduino_getGreeting);
m3ApiRawFunction(m3_clearLeds);
m3ApiRawFunction(m3_fillLeds);
m3ApiRawFunction(m3_setLed);
m3ApiRawFunction(m3_setLedRGB);
m3ApiRawFunction(m3_setLedHSV);