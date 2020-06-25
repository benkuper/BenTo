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
    {5,2,4},
    {14,12,13},
};

