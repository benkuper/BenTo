#pragma once

#define DEVICE_TYPE "Whack Button"

#ifndef ESP8266
#define ESP8266
#endif

// main leds config
#define LED_COUNT 6
//#define LED_EN_PIN 26 // pin for enabling the LED power. HIGH for on and LOW for off. 
#define LED_DATA_PIN 0
//#define LED_CLK_PIN 18
#define LED_TYPE    WS2812B
#define LED_COLOR_ORDER GRB


#define LED_MAX_BRIGHTNESS 150
#define LED_DEFAULT_BRIGHTNESS 255

// #define HAS_FILES
// #define FILES_USE_INTERNAL_MEMORY


// button pin
#define BUTTON_COUNT 1
#define BUTTON_INPUT_MODE INPUT_PULLUP
#define BUTTON_INVERTED
const int buttonPins[BUTTON_COUNT]{ 2 };    

#define NO_SLEEP_BUTTON


#define BoardPreInit \
{ \
pinMode(0, OUTPUT); \
digitalWrite(0, HIGH); \
delay(50); \
}

// #define SerialInit Serial.begin(115200, SERIAL_8N1, SERIAL_RX_ONLY);