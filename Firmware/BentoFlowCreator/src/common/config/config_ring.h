#pragma once

#ifndef CONFIG_FILE
#define CONFIG_FILE

#define DEVICE_TYPE "Ring"

#ifndef ESP32
#define ESP32
#endif

// main leds config
#define LED_COUNT 116
#define LED_DATA_PIN 15
#define LED_TYPE WS2812B
#define LED_COLOR_ORDER GRB
#define LED_MAX_BRIGHTNESS 200
#define LED_DEFAULT_BRIGHTNESS .5

//SLEEP
#define SLEEP_WAKEUP_BUTTON GPIO_NUM_27
#define SLEEP_WAKEUP_STATE HIGH

// button pin
#define BUTTON_INVERTED
#define BUTTON_COUNT 1
#define BUTTON_INPUT_MODE INPUT_PULLDOWN
const int buttonPins[BUTTON_COUNT]{ 27 };


#define HIGH_PIN_COUNT 1
const int highPins[HIGH_PIN_COUNT] { 26 };

//apparently helps having stable fastLED with TinyPICO and WS2812C
#define FASTLED_FORCE_LOOPDELAY

#define HAS_FILES
#define FILES_USE_INTERNAL_MEMORY


#define CAPACITIVE_COUNT 7

#endif