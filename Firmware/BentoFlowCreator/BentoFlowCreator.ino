#include "src/MainManager.h"

MainManager main("Flowtoys Creator Club", "2.0");

void setup()
{
  main.init();
}

void loop()
{
  main.update();
}