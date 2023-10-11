#include <Arduino.h>

#define MAIN_INCLUDE
#include "UnityIncludes.h"

extern "C" {
#include <esp_spiram.h>
#include <esp_himem.h>
}

RootComponent root("root");

void setup()
{
	root.init();
    DBG("Device is init");

	DBG("PSRAm size "+String(esp_spiram_get_size()));
	DBG("Size of component " + String(sizeof(Component)));
}

void loop()
{
	root.update();
}