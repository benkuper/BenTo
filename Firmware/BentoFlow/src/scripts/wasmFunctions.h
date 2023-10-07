#pragma once

#include "common/Common.h"

#ifdef HAS_SCRIPTS

#include <wasm3.h>
#include <m3_env.h>

m3ApiRawFunction(m3_arduino_millis);
m3ApiRawFunction(m3_arduino_getTime);
m3ApiRawFunction(m3_arduino_delay);
m3ApiRawFunction(m3_printFloat);
m3ApiRawFunction(m3_printInt);

#ifdef LED_COUNT
m3ApiRawFunction(m3_clearLeds);
m3ApiRawFunction(m3_dimLeds);
m3ApiRawFunction(m3_fillLeds);
m3ApiRawFunction(m3_fillLedsRGB);
m3ApiRawFunction(m3_fillLedsHSV);
m3ApiRawFunction(m3_setLed);
m3ApiRawFunction(m3_getLed);
m3ApiRawFunction(m3_setLedRGB);
m3ApiRawFunction(m3_setLedHSV);
m3ApiRawFunction(m3_pointRGB);
m3ApiRawFunction(m3_pointHSV);
m3ApiRawFunction(m3_setIR);
m3ApiRawFunction(m3_updateLeds);

m3ApiRawFunction(m3_getFXSpeed);
m3ApiRawFunction(m3_getFXIsoSpeed);
m3ApiRawFunction(m3_getFXStaticOffset);
m3ApiRawFunction(m3_getFXFlipped);
m3ApiRawFunction(m3_setFXSpeed);
m3ApiRawFunction(m3_setFXIsoSpeed);
m3ApiRawFunction(m3_setFXIsoAxis);
m3ApiRawFunction(m3_setFXStaticOffset);
m3ApiRawFunction(m3_resetFX);
#endif

m3ApiRawFunction(m3_getOrientation);
m3ApiRawFunction(m3_getYaw);
m3ApiRawFunction(m3_getPitch);
m3ApiRawFunction(m3_getRoll);
m3ApiRawFunction(m3_getThrowState);
m3ApiRawFunction(m3_getProjectedAngle);
m3ApiRawFunction(m3_setProjectedAngleOffset);
m3ApiRawFunction(m3_calibrateIMU);
m3ApiRawFunction(m3_setIMUEnabled);
m3ApiRawFunction(m3_getButtonState);
m3ApiRawFunction(m3_getActivity);
m3ApiRawFunction(m3_getSpin);
m3ApiRawFunction(m3_setMicEnabled);
m3ApiRawFunction(m3_getMicLevel);
m3ApiRawFunction(m3_setBatterySendEnabled);

m3ApiRawFunction(m3_randomInt);
m3ApiRawFunction(m3_noise);

#endif