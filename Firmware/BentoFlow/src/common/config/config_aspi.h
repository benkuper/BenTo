#pragma once

#ifndef CONFIG_FILE
#define CONFIG_FILE

#define DEVICE_TYPE "Aspi"

#ifndef ESP32
#define ESP32
#endif

// main leds config
#define LED_COUNT 104 + 122 // 104 leds dans le corps, 122 leds dans le tube
#define LED_DATA_PIN 26
#define LED_TYPE WS2812B
#define LED_COLOR_ORDER GRB
#define LED_MAX_BRIGHTNESS 200
#define LED_DEFAULT_BRIGHTNESS .2
//#define LED_USE_FET
//#define LED_FET_PIN 33


#define GENERATE_LED_INDEX_MAP
#define STRIP_LED_COUNT 26
#define NUM_STRIPS 4
#define NUM_LEDS_IN_BODY STRIP_LED_COUNT * NUM_STRIPS


void generateLedIndexMap();
int getLedAtIndex(int index);

#define LEDMAP(index) getLedAtIndex(index)

//SLEEP
#define BUTTON_INVERTED
#define SLEEP_WAKEUP_BUTTON GPIO_NUM_39
#define SLEEP_WAKEUP_STATE LOW

// button pin
#define BUTTON_COUNT 2
#define BUTTON_INPUT_MODE INPUT_PULLUP
const int buttonPins[BUTTON_COUNT]{ 39, 37 };

//apparently helps having stable fastLED with TinyPICO and WS2812C
#define FASTLED_FORCE_LOOPDELAY

#define HAS_FILES
#define FILES_USE_INTERNAL_MEMORY

#endif