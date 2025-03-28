#pragma once

#ifndef CONFIG_FILE
#define CONFIG_FILE

#define DEVICE_TYPE "Lamp"

#ifndef ESP32
#define ESP32
#endif

// main leds config
#define LED_COUNT 30
#define LED_DATA_PIN 5
#define LED_TYPE WS2812B
#define LED_COLOR_ORDER GRB
#define LED_MAX_BRIGHTNESS 200
#define LED_DEFAULT_BRIGHTNESS .5
#define LED_USE_FET
#define LED_FET_PIN 33

//SLEEP
#define BUTTON_INVERTED
#define SLEEP_WAKEUP_BUTTON GPIO_NUM_4
#define SLEEP_WAKEUP_STATE LOW

// button pin
 #define BUTTON_COUNT 1
 #define BUTTON_INPUT_MODE INPUT_PULLUP
 const int buttonPins[BUTTON_COUNT]{ 4 };

//#define TOUCH_COUNT 1
//const int touchPins[TOUCH_COUNT]{4};
//#define TOUCH_WAKEUP_PIN T0 //TOUCH_PAD_NUM0 //touch pad 0 is pin 4

//apparently helps having stable fastLED with TinyPICO and WS2812C
#define FASTLED_FORCE_LOOPDELAY

#define HAS_FILES
#define FILES_USE_INTERNAL_MEMORY

#define GROUND_PIN_COUNT 1
const int groundPins[GROUND_PIN_COUNT]{ 14 };

//TinyPICO code

// Battery divider resistor values
#define UPPER_DIVIDER 442
#define LOWER_DIVIDER 160
#define DEFAULT_VREF 1100           // Default reference voltage in mv
#define BATT_CHANNEL ADC1_CHANNEL_7 // Battery voltage ADC input

//battery
#define BATTERY_CHARGE_PIN 34
#define BATTERY_PIN 35
#define BATTERY_VOLTAGE_MIN 3.5f
#define BATTERY_VOLTAGE_MAX 4.2f

#define CUSTOM_BATTERY_READ

#include "driver/adc.h"
#include "esp_adc_cal.h"

int getBatteryRawValue();
float getBatteryVoltage(int raw);

bool isChargingBattery();


#define DOTSTAR_PWR 13
#define DOTSTAR_DATA 2
#define DOTSTAR_CLK 12

#define BoardInit \
digitalWrite( DOTSTAR_PWR, true ); \
pinMode( DOTSTAR_DATA, INPUT_PULLDOWN ); \
pinMode( DOTSTAR_CLK,  INPUT_PULLDOWN );

#endif