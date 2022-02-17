#include <Arduino.h>

#define MAIN_INCLUDE
#include "UnityIncludes.h"

RootComponent root;
// Script s;

void setup()
{
	root.init();
	delay(1000);
	//s.load("test");
}

void loop()
{
	root.update();
}