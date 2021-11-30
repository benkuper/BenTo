#pragma once

#pragma message ("Compiling For " ARDUINO_BOARD)


#ifdef ARDUINO_FLOWTOYS_CREATOR
#pragma message ("Flowtoys Edition")
#endif

#ifdef POWER_PIN 12
//keep pin12 high to keep board awake
#define BoardInit \
    pinMode(12,OUTPUT);\
    digitalWrite(12, HIGH);

#endif




#ifndef BoardInit
#define BoardInit 
#endif