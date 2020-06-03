#pragma once

#define DEVICE_TYPE "Glove"
#define ESP32

#define BUTTON_COUNT 6
#define BUTTON_INPUT_MODE INPUT_PULLUP
#define BUTTON_REF_PULLDOWN 13
const int buttonPins[BUTTON_COUNT]{ 23, 2, 12,21,27,14}; //4 last buttons are glove, 26 is palm but not working right now 

#define BUTTON_INVERTED
#define SLEEP_WAKEUP_BUTTON GPIO_NUM_2
#define SLEEP_WAKEUP_STATE LOW

#define HAS_IMU
//#define I2C_CLOCK 10000 // not sure what this is for.
#define SDA_PIN 4 // i2c lines
#define SCL_PIN 15 // i2c lines
#define IMU_REMAP_CONFIG Adafruit_BNO055::REMAP_CONFIG_P2
#define IMU_REMAP_SIGN Adafruit_BNO055::REMAP_SIGN_P2

#define INT_PIN 0 // the interupt pin for the IMU MPU prosessor.
#define IMU_RESET 0 // toggl this pin to reset the IMU before connectiong to it after a reset.
#define BNO055_SAMPLERATE_DELAY_MS (100)

#define HAS_DISPLAY
#define DISPLAY_LINES 4
#define DISPLAY_SDA 4
#define DISPLAY_SCL 15
#define DISPLAY_RESET 16
#define DISPLAY_REFRESH_RATE 20