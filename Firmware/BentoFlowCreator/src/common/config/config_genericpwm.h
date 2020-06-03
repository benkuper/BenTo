#pragma once

#define DEVICE_TYPE "Generic PWM"
#define ESP8266

#define BUTTON_COUNT 0

// main leds config
#define LED_COUNT 2
#define LED_SEPARATE_CHANNELS 1

#define HAS_FILES
#define FILES_USE_INTERNAL_MEMORY

struct RGBLedPins
{
  int rPin;
  int gPin;
  int bPin;
};

const RGBLedPins rgbLedPins[LED_COUNT]
{
    {D5,D6,D7},
    {D1,D2,D3},
};

