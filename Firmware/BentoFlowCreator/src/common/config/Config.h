#pragma once
#include "Arduino.h"

// #define CREATOR_PROTO_V0
// #define CREATOR_PROTO_V1
//#define CREATOR_PROTO_V2
// #define LT_BALL
 //#define LT_HOOP
 ///#define CLARINET
 //#define SPECTRA_GLOVE
 #define GENERIC_PWM

#if defined CREATOR_PROTO_V0
#include "config_protov0.h"
#elif defined CREATOR_PROTO_V1
#include "config_protov1.h"
#elif defined CREATOR_PROTO_V2
#include "config_protov2.h"
#elif defined LT_HOOP
#include "config_lthoop.h"
#elif defined LT_BALL
#include "config_ltball.h"
#elif defined CLARINET
#include "config_clarinet.h"
#elif defined SPECTRA_GLOVE
#include "config_spectraglove.h"
#elif defined GENERIC_PWM
#include "config_genericpwm.h"
#endif

#ifdef LED_INVERT_DIRECTION
    #define LEDMAP(index) (LED_COUNT-1-index)
#else
    #define LEDMAP(index) index
#endif

#pragma message("Compiling for " __DEVICE_TYPE__)

