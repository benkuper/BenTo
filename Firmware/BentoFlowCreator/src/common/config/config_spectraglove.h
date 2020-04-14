#pragma once

#define DEVICE_TYPE "Spectra Glove"

#define ESP32

#define BUTTON_COUNT 5
#define BUTTON_INPUT_MODE INPUT_PULLUP
const int buttonPins[BUTTON_COUNT]{ A2, A3, A4, A11, A7 };
#define BUTTON_INVERTED
#define SLEEP_WAKEUP_BUTTON GPIO_NUM_26

// main leds config
#define LED_COUNT 5
#define LED_SEPARATE_CHANNELS 1
#define LED_PWM_FREQUENCY 5000
#define LED_PWM_RESOLUTION 10

struct RGBLedPins
{
  int rPin;
  int gPin;
  int bPin;
};

const RGBLedPins rgbLedPins[LED_COUNT]
{
    {14,22,23},
    {27,33,15},
    {5,18,19},
    {A0,A1,A5},
    {16,17,21}
};

