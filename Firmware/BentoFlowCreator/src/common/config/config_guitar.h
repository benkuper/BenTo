#pragma once

#ifndef CONFIG_FILE
#define CONFIG_FILE

#define DEVICE_TYPE "Guitar"

#ifndef ESP32
#define ESP32
#endif

// main leds config
#define LED_COUNT 324
#define LED_DATA_PIN 15
#define LED_TYPE WS2812B
#define LED_COLOR_ORDER GRB
#define LED_MAX_BRIGHTNESS 200
#define LED_DEFAULT_BRIGHTNESS .5

//SLEEP
#define BUTTON_INVERTED
#define SLEEP_WAKEUP_BUTTON GPIO_NUM_4
#define SLEEP_WAKEUP_STATE LOW

// button pin
#define BUTTON_COUNT 2
#define BUTTON_INPUT_MODE INPUT_PULLUP
const int buttonPins[BUTTON_COUNT]{ 4, 14 };

//apparently helps having stable fastLED with TinyPICO and WS2812C
#define FASTLED_FORCE_LOOPDELAY

#define HAS_FILES
#define FILES_USE_INTERNAL_MEMORY

#endif