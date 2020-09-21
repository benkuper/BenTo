#pragma once

//  #define CREATOR_PROTO_V0
//  #define CREATOR_PROTO_V1
// #define CREATOR_PROTO_V2
//  #define LT_BALL
//  #define LT_HOOP
//  #define CLARINET
//  #define GLOVE
//  #define GENERIC_PWM
  #define DMX
//    #define DRESS
//#define FLAIR

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
#elif defined GLOVE
#include "config_glove.h"
#elif defined GENERIC_PWM
#include "config_genericpwm.h"
#elif defined DMX
#include "config_dmx.h"
#elif defined DRESS
#include "config_dress.h"
#elif defined FLAIR
#include "config_flair.h"
#endif

#ifdef LED_INVERT_DIRECTION
    #define LEDMAP(index) (LED_COUNT-1-index)
#else
    #define LEDMAP(index) index
#endif

//#pragma message("Compiling for " __DEVICE_TYPE__)

