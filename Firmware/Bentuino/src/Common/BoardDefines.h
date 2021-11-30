#pragma once

#define STR(x) #x
#define XSTR(x) STR(x)

#ifndef MAIN_INCLUDE //Avoid showing twice
#pragma message ("Compiling For " ARDUINO_BOARD)
#endif

#ifdef POWER_KEEP_PIN

#ifndef MAIN_INCLUDE //Avoid showing twice
#pragma message ("Using Power PIN " XSTR(POWER_KEEP_PIN))
#endif

//keep pin12 high to keep board awake
#define BoardInit \
    pinMode(POWER_KEEP_PIN, OUTPUT);\
    digitalWrite(POWER_KEEP_PIN, HIGH);

#endif




#ifndef BoardInit
#define BoardInit 
#endif