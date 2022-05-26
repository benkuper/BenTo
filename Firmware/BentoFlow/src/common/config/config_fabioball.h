#pragma once

#ifndef CONFIG_FILE
#define CONFIG_FILE

#define DEVICE_TYPE "Fabio Ball"


#ifndef ESP32
#define ESP32
#endif


// main leds config
#define LED_COUNT 64
#define LED_NUM_STRIPS 4

#define LED_COUNT1 16
#define LED_DATA_PIN1 27
#define LED_CLK_PIN1 32

#define LED_COUNT2 16
#define LED_DATA_PIN2 25
#define LED_CLK_PIN2 32

#define LED_COUNT3 16
#define LED_DATA_PIN3 4
#define LED_CLK_PIN3 32

#define LED_COUNT4 16
#define LED_DATA_PIN4 12
#define LED_CLK_PIN4 32

#define LED_TYPE APA102
#define LED_COLOR_ORDER BGR
#define LED_MAX_BRIGHTNESS 50
#define LED_DEFAULT_BRIGHTNESS .5

//apparently helps having stable fastLED with TinyPICO and WS2812C
#define FASTLED_FORCE_LOOPDELAY

#define HAS_FILES
#define FILES_USE_INTERNAL_MEMORY

#define GENERATE_LED_INDEX_MAP
#define STRIP_LED_COUNT 16
#define NUM_STRIPS 4

void generateLedIndexMap();
int getLedAtIndex(int index);

#define LEDMAP(index) getLedAtIndex(index)

//battery
#endif