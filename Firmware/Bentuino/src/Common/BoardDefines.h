#pragma once



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

#define BoardShutdown \
    digitalWrite(POWER_KEEP_PIN, LOW);
#endif



#ifndef BoardInit
#define BoardInit 
#endif

#ifndef BoardShutdown
#define BoardShutdown
#endif