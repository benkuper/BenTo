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
 //#define SAX
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
#include "config_creatorclub.h"
#elif defined ARDUINO_FLOWTOYS_CREATOR_HOOP
#include "config_creator_hoop.h"
#elif defined ARDUINO_CREATOR_WHEEL
#include "config_creatorwheel.h"
#elif defined LT_HOOP
#include "config_lthoop.h"
#elif defined LT_BALL
#include "config_ltball.h"
#elif defined ARDUINO_CLARINET
#include "config_clarinet.h"
#elif defined ARDUINO_SAX
#include "config_sax.h"
#elif defined ARDUINO_GUITAR
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
#elif defined ARDUINO_COSTUME
#include "config_costume.h"
#elif defined ARDUINO_ASPI
#include "config_aspi.h"
#elif defined ARDUINO_BIKE
#include "config_bike.h"
#elif defined ARDUINO_SIMPLE_ESP32
#include "config_simpleESP32.h"
#elif defined ARDUINO_M5_TESTER
#include "config_m5tester.h"
#elif defined ARDUINO_FABIO_BALL
#include "config_fabioball.h"
#elif defined ARDUINO_FLOORSTRIP
#include "config_floorstrip.h"
#elif defined ARDUINO_ESP32_POE_ISO
#include "config_olimextester.h"
#elif defined ARDUINO_OLIMEX_TUBE
#include "config_olimextube.h"
#elif defined ARDUINO_TAP_DANCE
#include "config_tab_tap.h"
#elif defined ARDUINO_WHACK_BUTTON
#include "config_whackbutton..h"
#endif

#ifdef LED_INVERT_DIRECTION
    #define LEDMAP(index) (LED_COUNT-1-index)
#elif !defined LEDMAP
    #define LEDMAP(index) index
#endif


#ifndef BoardPreInit
#define BoardPreInit
#endif

#ifndef BoardInit
#define BoardInit
#endif

#ifndef BoardShutdown
#define BoardShutdown
#endif

//#pragma message("Compiling for " ARDUINO_BOARD " (" DEVICE_TYPE ") ")

#endif