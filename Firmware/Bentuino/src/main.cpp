#include <Arduino.h>

#define MAIN_INCLUDE
#include "UnityIncludes.h"

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