#pragma once

#ifndef CONFIG_H
#define CONFIG_H

//  #define CREATOR_PROTO_V0
//  #define CREATOR_PROTO_V1
// #define CREATOR_PROTO_V2
// #define CREATOR_TAB_HOOP
//  #define LT_BALL
//  #define LT_HOOP
//  #define CLARINET
// #define SAX
// #define GUITAR
//  #define GLOVE
//  #define GENERIC_PWM
//  #define DMX
//  #define DRESS
//  #define FLAIR
//  #define SIMPLE_ESP8266
//  #define SIMPLE_ESP32
//  #define RING
//  #define SUPERSTAFF
// #define LAMP
//  #define ASPI
//  #define POWERLED
//  #define BIKE

//#define CYRWHEEL


//#define BOX
//#define MAKEY

#if defined CREATOR_PROTO_V0
#include "config_protov0.h"
#elif defined CREATOR_PROTO_V1
#include "config_protov1.h"
#elif defined ARDUINO_FLOWTOYS_CREATOR_CLUB
#include "config_protov2.h"
#elif defined CREATOR_TAB_HOOP
#include "config_tab_hoop.h"
#elif defined LT_HOOP
#include "config_lthoop.h"
#elif defined LT_BALL
#include "config_ltball.h"
#elif defined CLARINET
#include "config_clarinet.h"
#elif defined SAX
#include "config_sax.h"
#elif defined GUITAR
#include "config_guitar.h"
#elif defined BOX
#include "config_box.h"
#elif defined MAKEY
#include "config_makey.h"
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
#elif defined SIMPLE_ESP8266
#include "config_simpleESP8266.h"
#elif defined RING
#include "config_ring.h"
#elif defined ARDUINO_FLOWTOYS_CREATOR_SUPERSTAFF
#include "config_tab_superstaff.h"
#elif defined POWERLED
#include "config_tab_powerled.h"
#elif defined CYRWHEEL
#include "config_tab_cyrwheel.h"
#elif defined ARDUINO_FLOWTOYS_CREATOR_LIGHTBULB
#include "config_lamp.h"
#elif defined ASPI
#include "config_aspi.h"
#elif defined ARDUINO_BIKE
#include "config_bike.h"
#elif defined SIMPLE_ESP32
#include "config_simpleESP32.h"
#elif defined ARDUINO_M5_TESTER
#include "config_m5tester.h"
#endif

#ifdef LED_INVERT_DIRECTION
    #define LEDMAP(index) (LED_COUNT-1-index)
#elif !defined LEDMAP
    #define LEDMAP(index) index
#endif

//#pragma message("Compiling for " ARDUINO_BOARD " (" DEVICE_TYPE ") ")

#endif