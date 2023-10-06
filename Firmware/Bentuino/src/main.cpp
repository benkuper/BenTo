#include <Arduino.h>

#define MAIN_INCLUDE
#include "UnityIncludes.h"

RootComponent root("root");

void setup()
{
	root.init();
    DBG("Device is init, free heap " + String(ESP.getFreeHeap()) + " / " + String(ESP.getMinFreeHeap()));
}

void loop()
{
	root.update();
}