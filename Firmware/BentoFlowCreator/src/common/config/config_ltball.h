#pragma once

#define DEVICE_TYPE "Lightrix Ball"

// main leds config
#define LED_COUNT 5
#define LED_DATA_PIN 13
#define LED_CLK_PIN 14
#define LED_TYPE    APA102
#define LED_COLOR_ORDER BGR

//main power
#define SLEEP_WAKEUP_BUTTON GPIO_NUM_13

// button pin
#define BUTTON_PIN 4 // pin for sensing the button press.