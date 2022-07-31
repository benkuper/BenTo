#include "Config.h"
#ifdef ARDUINO_SAX
#include <Arduino.h>


int getBatteryRawValue()
{
    analogRead(BATTERY_PIN); // Just to get the ADC setup
    return adc1_get_raw(BATT_CHANNEL);
}

float getBatteryVoltage(int raw)
{
    esp_adc_cal_characteristics_t chars;

    // Get ADC calibration values
    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_11db, ADC_WIDTH_BIT_12, DEFAULT_VREF, &chars);

    // Convert to calibrated mv then volts
    uint32_t mv = esp_adc_cal_raw_to_voltage(raw, &chars) * (LOWER_DIVIDER + UPPER_DIVIDER) / LOWER_DIVIDER;
    return (float)mv / 1000.0;
}

bool isChargingBattery()
{
    int measuredVal = 0;
    for (int i = 0; i < 10; i++)
    {
        int v = digitalRead(BATTERY_CHARGE_PIN);
        measuredVal += v;
    }

    return (measuredVal == 0);
}

#endif