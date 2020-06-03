#pragma once

#define DEVICE_TYPE "Lightrix Hoop"
#define ESP8266

// main leds config
#define LED_COUNT 250
#define LED_DATA_PIN 13
#define LED_CLK_PIN 14
#define LED_TYPE    APA102
#define LED_COLOR_ORDER BGR

// button pin
#define BUTTON_COUNT 1
#define BUTTON_INPUT_MODE INPUT
const int buttonPins[BUTTON_COUNT]{ 5 };
#define BUTTON_INVERTED

#define SLEEP_PIN 5
#define SLEEP_PIN_SLEEP_VAL LOW
#define INFINITE_LOOP_ON_SLEEP