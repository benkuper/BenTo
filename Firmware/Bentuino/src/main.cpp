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
}

void loop()
{
	root.update();
}