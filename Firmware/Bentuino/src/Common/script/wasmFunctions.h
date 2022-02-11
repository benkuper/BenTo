#pragma once

m3ApiRawFunction(m3_arduino_millis);
m3ApiRawFunction(m3_arduino_delay);
m3ApiRawFunction(m3_logUTF16);
m3ApiRawFunction(m3_printFloat);
m3ApiRawFunction(m3_printInt);
m3ApiRawFunction(m3_randomInt);
m3ApiRawFunction(m3_noise);

m3ApiRawFunction(m3_setInt);
m3ApiRawFunction(m3_setFloat);

void handleM3SetVal(const uint8_t * buf, uint32_t len, var val);