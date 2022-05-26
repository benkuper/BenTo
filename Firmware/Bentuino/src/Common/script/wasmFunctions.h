#pragma once

m3ApiRawFunction(m3_arduino_millis);
m3ApiRawFunction(m3_arduino_delay);
m3ApiRawFunction(m3_logUTF16);
m3ApiRawFunction(m3_printFloat);
m3ApiRawFunction(m3_printInt);
m3ApiRawFunction(m3_randomInt);
m3ApiRawFunction(m3_noise);

m3ApiRawFunction(m3_getIntUTF16);
m3ApiRawFunction(m3_getFloatUTF16);
m3ApiRawFunction(m3_setIntUTF16);
m3ApiRawFunction(m3_setFloatUTF16);

Parameter * getParameterFromM3(const uint8_t * buf, uint32_t len);