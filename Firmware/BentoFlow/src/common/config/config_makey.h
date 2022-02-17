#pragma once

#ifndef CONFIG_FILE
#define CONFIG_FILE

#define DEVICE_TYPE "Makey"

#ifndef ESP32
#define ESP32
#endif

// main leds config
#define LED_COUNT 1
#define LED_TYPE WS2812B
#define LED_DATA_PIN 15
#define LED_COLOR_ORDER RGB

// button pin
#define NO_SLEEP_BUTTON

#define BUTTON_INVERTED
#define BUTTON_COUNT 1
#define BUTTON_INPUT_MODE INPUT
const int buttonPins[BUTTON_COUNT]{ 25 };

#define HIGH_PIN_COUNT 2
const int highPins[HIGH_PIN_COUNT] { 26, 13};

#endif