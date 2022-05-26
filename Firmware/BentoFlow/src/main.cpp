#include <Arduino.h>
#include "MainManager.h"
MainManager mainManager("1.2.4");

void setup()
{
  delay(50); //
  mainManager.init();
}

void loop()
{
  mainManager.update();
}
