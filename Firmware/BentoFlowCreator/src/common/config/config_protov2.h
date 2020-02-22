#pragma once

#define DEVICE_TYPE "Flowtoys Creator Club"

// main leds config
#define LED_COUNT 32
#define LED_EN_PIN 27 // pin for enabling the LED power. HIGH for on and LOW for off. 
#define LED_DATA_PIN 25
#define LED_CLK_PIN 26
#define LED_TYPE    SK9822
#define LED_COLOR_ORDER BGR

//main power
#define SLEEP_PIN 12 // pin for turning the LDO on and off. LOW for off and HIGH for on. 
#define SLEEP_WAKEUP_BUTTON GPIO_NUM_32
//setting low will colapse the power and the club will only turn on from USB or button press.

// button pin
#define BUTTON_PIN 32 // pin for sensing the button press.

//battery sence
#define BATTERY_PIN 35 // takes the measurment from the battery sence. 

// imu config
#define HAS_IMU 1
#define I2C_CLOCK 10000 // not sure what this is for.

#define SDA_PIN 23 // i2c lines
#define SCL_PIN 22 // i2c lines

#define INT_PIN 33 // the interupt pin for the IMU MPU prosessor.
#define IMU_RESET 21 // toggl this pin to reset the IMU before connectiong to it after a reset.
#define BNO055_SAMPLERATE_DELAY_MS (100)

// ir config
#define IR_TX_PIN 17 // drive with PWM HIGH is on and LOW is off.

// SD config
#define SDSPEED 27000000

#define SD_EN 16 // drive LOW to turn on and set to HIGH-Z for off. 

#define SD_MISO 19
#define SD_MOSI 13
#define SD_SCK 14
#define SD_CS 15