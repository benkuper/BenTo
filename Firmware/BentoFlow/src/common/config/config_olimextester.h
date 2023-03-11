#pragma once

#ifndef CONFIG_FILE
#define CONFIG_FILE

#define DEVICE_TYPE "Olimex Tester"

#ifndef ESP32
#define ESP32
#endif

// main leds config
#define LED_COUNT 300
// #define LED_EN_PIN 27 // pin for enabling the LED power. HIGH for on and LOW for off. 
#define LED_DATA_PIN 32
#define LED_CLK_PIN 33
#define LED_TYPE    SK9822
#define LED_COLOR_ORDER BGR
#define LED_MAX_BRIGHTNESS 60
#define LED_DEFAULT_BRIGHTNESS .5
//#define LED_INVERT_DIRECTION

//SLEEP
#define BUTTON_INVERTED
//#define SLEEP_WAKEUP_BUTTON GPIO_NUM_4
//#define SLEEP_WAKEUP_STATE LOW

// button pin
// #define BUTTON_COUNT 2
// #define BUTTON_INPUT_MODE INPUT_PULLUP
// const int buttonPins[BUTTON_COUNT]{ 37, 39 };

/*
#define TOUCH_COUNT 2
const int touchPins[TOUCH_COUNT]{37, 39};
#define TOUCH_WAKEUP_PIN T0 //TOUCH_PAD_NUM0 //touch pad 0 is pin 4
*/

//apparently helps having stable fastLED with TinyPICO and WS2812C
// #define FASTLED_FORCE_LOOPDELAY

// SD config
#define HAS_FILES
#define USE_SD_MMC

//#define GROUND_PIN_COUNT 2
//const int groundPins[GROUND_PIN_COUNT]{13};

//battery
#endif