#pragma once

#ifndef CONFIG_FILE
#define CONFIG_FILE

#define DEVICE_TYPE "Floor Strip"

#ifndef ESP32
#define ESP32
#endif

// main leds config
#define LED_COUNT 439
#define LED_DATA_PIN 32
#define LED_TYPE WS2812B
#define LED_COLOR_ORDER GRB
#define LED_MAX_BRIGHTNESS 150
#define LED_DEFAULT_BRIGHTNESS .3
// #define LED_USE_FET
// #define LED_FET_PIN 33

//#define USE_MIC 1

//battery
/*#define BATTERY_CHARGE_PIN 34
#define BATTERY_PIN 35
#define BATTERY_DEFAULT_MIN 880
#define BATTERY_DEFAULT_MAX 1100
*/

#define GROUND_PIN_COUNT 1
const int groundPins[GROUND_PIN_COUNT]{ 25 };

//SLEEP
#define BUTTON_INVERTED
#define SLEEP_WAKEUP_BUTTON GPIO_NUM_37
#define SLEEP_WAKEUP_STATE LOW

// button pin
#define BUTTON_COUNT 2
#define BUTTON_INPUT_MODE INPUT_PULLUP
const int buttonPins[BUTTON_COUNT]{ 37,39 };

//apparently helps having stable fastLED with TinyPICO and WS2812C
#define FASTLED_FORCE_LOOPDELAY

#define HAS_FILES
#define FILES_USE_INTERNAL_MEMORY

#endif