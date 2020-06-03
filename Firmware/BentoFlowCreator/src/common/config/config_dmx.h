#pragma once

#define DEVICE_TYPE "DMX"
#define ESP8266

#define BUTTON_COUNT 0

// main leds config
#define LED_COUNT 1
#define LED_USE_DMX 1
const int dmxStartChannels[LED_COUNT] {1}

#define HAS_FILES
#define FILES_USE_INTERNAL_MEMORY