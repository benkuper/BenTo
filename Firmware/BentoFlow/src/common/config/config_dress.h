#pragma once

#define DEVICE_TYPE "Dress"

#ifndef ESP32
#define ESP32
#endif

// main leds config
#define LED_COUNT 1860
#define LED_NUM_STRIPS 5

#define LED_PIN1 12 //purple
#define LED_PIN2 32 //blue
#define LED_PIN3 19 //green
#define LED_PIN4 27 //grey
#define LED_PIN5 33 //white

#define LED_COUNT1 119 //shoulderLeft
#define LED_COUNT2 119 //shoulderRight
#define LED_COUNT3 328 //crown
#define LED_COUNT4 742 //body back > front
#define LED_COUNT5 552 //skirt

#define LED_START1 0
#define LED_START2 LED_START1 + LED_COUNT1
#define LED_START3 LED_START2 + LED_COUNT2
#define LED_START4 LED_START3 + LED_COUNT3
#define LED_START5 LED_START4 + LED_COUNT4
#define LED_START6 LED_START5 + LED_COUNT5

#define LED_COLOR_ORDER GRB
#define LED_TYPE WS2812B

#define BUTTON_COUNT 0

#define HAS_FILES
#define SDSPEED 10000000
#define SD_MISO 21
#define SD_MOSI 13
#define SD_SCK 14
#define SD_CS 15