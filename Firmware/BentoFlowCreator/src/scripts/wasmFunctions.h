#pragma once

#include <wasm3.h>
#include <m3_env.h>

m3ApiRawFunction(m3_arduino_millis);
m3ApiRawFunction(m3_arduino_delay);
m3ApiRawFunction(m3_arduino_print);
m3ApiRawFunction(m3_clearLeds);
m3ApiRawFunction(m3_fillLeds);
m3ApiRawFunction(m3_fillLedsRGB);
m3ApiRawFunction(m3_fillLedsHSV);
m3ApiRawFunction(m3_setLed);
m3ApiRawFunction(m3_setLedRGB);
m3ApiRawFunction(m3_setLedHSV);
m3ApiRawFunction(m3_pointRGB);
m3ApiRawFunction(m3_pointHSV);
m3ApiRawFunction(m3_getOrientation);
m3ApiRawFunction(m3_getYaw);
m3ApiRawFunction(m3_getPitch);
m3ApiRawFunction(m3_getRoll);
m3ApiRawFunction(m3_getNumber);
m3ApiRawFunction(m3_getThrowState);
m3ApiRawFunction(m3_updateLeds);