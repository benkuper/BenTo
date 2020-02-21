#pragma once

#define DEVICE_TYPE "Flowtoys Creator Club"

// main leds config
#define LED_COUNT 32
#define LED_EN_PIN 26 // pin for enabling the LED power. HIGH for on and LOW for off. 
#define LED_DATA_PIN 5
#define LED_CLK_PIN 18
#define LED_TYPE    SK9822
#define LED_COLOR_ORDER BGR

//main power
#define SLEEP_WAKEUP_BUTTON GPIO_NUM_13

// button pin
#define BUTTON_PIN 23 // pin for sensing the button press.

//battery sence
#define HAS_BATTERY_SENSING 0

// imu config
#define HAS_IMU 0

// ir config
#define IR_TX_PIN 17 // drive with PWM HIGH is on and LOW is off.

// SD config
#define SDSPEED 27000000

#define SD_EN 32 // drive LOW to turn on and set to HIGH-Z for off. 

#define SD_MISO 27
#define SD_MOSI 25
#define SD_SCK 33
#define SD_CS 2