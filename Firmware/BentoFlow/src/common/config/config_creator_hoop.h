#pragma once

#define DEVICE_TYPE "Flowtoys Creator Hoop"

#ifndef ESP32
#define ESP32
#endif

// main leds config
#define LED_COUNT 155
#define LED_EN_PIN 27 // pin for enabling the LED power. HIGH for on and LOW for off. 
#define LED_DATA_PIN 25
#define LED_CLK_PIN 26
#define LED_TYPE SK9822
#define LED_COLOR_ORDER BGR
//#define LED_DATA_RATE DATA_RATE_KHZ(24)
//#define LED_INVERT_DIRECTION

//out of 1
#define LED_DEFAULT_BRIGHTNESS .2

//out of 255
#define LED_MAX_BRIGHTNESS 255

//main power
#define KEEP_SLEEP_PIN_HIGH
#define SLEEP_PIN 12 // pin for turning the LDO on and off. LOW for off and HIGH for on. 
#define SLEEP_PIN_SLEEP_VAL LOW
#define SLEEP_WAKEUP_BUTTON GPIO_NUM_36
#define SLEEP_WAKEUP_STATE HIGH

//setting low will colapse the power and the club will only turn on from USB or button press.

// button pin
#define BUTTON_COUNT 1
#define BUTTON_INPUT_MODE INPUT
const int buttonPins[BUTTON_COUNT]{ 36 };

//battery sence
#define BATTERY_PIN 35 // takes the measurment from the battery sence. 

#define HAS_IMU
//#define I2C_CLOCK 10000 // not sure what this is for.
#define SDA_PIN 23 // i2c lines
#define SCL_PIN 22 // i2c lines
#define IMU_REMAP_CONFIG Adafruit_BNO055::REMAP_CONFIG_P0
#define IMU_REMAP_SIGN Adafruit_BNO055::REMAP_SIGN_P0

#define INT_PIN 34 // the interupt pin for the IMU MPU prosessor.
#define IMU_RESET 21 // toggl this pin to reset the IMU before connectiong to it after a reset.
#define IMU_ADDR 0x28
#define BNO055_SAMPLERATE_DELAY_MS (100)

// ir config
//#define HAS_IR

// #define IR_TX_PIN 17 // drive with PWM HIGH is on and LOW is off.
// #define IR_MAX_POWER 25 // 10%
// #define IR_CHANNEL 0
// #define IR_PWM_RESOLUTION 8 //8 = value range from 0 to 255
// #define IR_FREQ 5000

// SD config
#define HAS_FILES

#define SDSPEED 27000000

#define SD_EN 33 // drive LOW to turn on and set to HIGH-Z for off. 
#define SD_POWER_VALUE LOW

#define SD_MISO 19
#define SD_MOSI 13
#define SD_SCK 14
#define SD_CS 15