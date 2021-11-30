#include <Arduino.h>

#define MAIN_INCLUDE
#include "UnityIncludes.h"

RootComponent root;

void setup()
{
	root.init();
}

void loop()
{
	root.update();
}
