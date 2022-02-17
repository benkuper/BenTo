#pragma once

#define DEVICE_TYPE "Flair"
#ifndef ESP8266
#define ESP8266
#endif

// main leds config
#define LED_COUNT 18
#define LED_DATA_PIN 13
//#define LED_CLK_PIN 14
#define LED_TYPE    WS2812B
#define LED_COLOR_ORDER BGR

//main power
//#define SLEEP_WAKEUP_BUTTON GPIO_NUM_13
//#define SLEEP_WAKEUP_STATE HIGH

// button pin
#define BUTTON_COUNT 1
#define BUTTON_INPUT_MODE INPUT
const int buttonPins[BUTTON_COUNT]{ 4 };
