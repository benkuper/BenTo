#include "config_fabioball.h"
#include <Arduino.h>
#include "../DebugHelpers.h"

#ifdef ARDUINO_FABIO_BALL

int ledIndexMap[LED_COUNT];

void generateLedIndexMap()
{
    int index = 0;
    for (int i = 0; i < STRIP_LED_COUNT; i++)
    {
        for (int j = 0; j < NUM_STRIPS; j++)
        {
            ledIndexMap[index] = i + j * 16;
            DBG("Led at index : " + String(index) + " > " + String(ledIndexMap[index]));
            index++;
        }
    }
    
    //

}

int getLedAtIndex(int index)
{
    return ledIndexMap[index];
}

// int ledIndexMap[LED_COUNT]{0, 16, 32, 48, 1, 17, 33, 49, 2, 18, 34, 50, 3, 19, 35, 51, 4, 20, 36, 52, 5, 21, 37, 53, 6, 22, 38, 54, 7, 23, 39, 55, 8, 24, 40, 56, 9, 25, 41, 57, 10, 26, 42, 58, 11, 27, 43, 59, 12, 28, 44, 60, 13, 29, 45, 61, 14, 30, 46, 62, 15, 31, 47, 63};

#endif