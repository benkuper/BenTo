#include <Arduino.h>
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
