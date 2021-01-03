#pragma once

#ifndef CONFIG_FILE
#define CONFIG_FILE

#define DEVICE_TYPE "Box"

#ifndef ESP32
#define ESP32
#endif

// main leds config
#define LED_COUNT 2
#define LED_SEPARATE_CHANNELS 1
#define LED_PWM_FREQUENCY 5000
#define LED_PWM_RESOLUTION 10
//#define PWMVAL(i) map(i, 0, 255, 0, 1023)

//SLEEP
#define SLEEP_WAKEUP_BUTTON GPIO_NUM_26
#define SLEEP_WAKEUP_STATE LOW

// button pin
#define BUTTON_INVERTED
#define BUTTON_INPUT_MODE INPUT_PULLUP


#define BUTTON_COUNT 2
#define BUTTON_INPUT_MODE INPUT_PULLUP
const int buttonPins[BUTTON_COUNT]{ 26, 32 };

#define GROUND_PIN_COUNT 1
const int groundPins[GROUND_PIN_COUNT] { 15 };

#define HIGH_PIN_COUNT 1
const int highPins[HIGH_PIN_COUNT] { 13 };

//apparently helps having stable fastLED with TinyPICO and WS2812C
//#define FASTLED_FORCE_LOOPDELAY
struct RGBLedPins
{
  int rPin;
  int gPin;
  int bPin;
};

const RGBLedPins rgbLedPins[LED_COUNT]
{
    {25, -1, -1},
    {14, -1, -1},
};

#endif