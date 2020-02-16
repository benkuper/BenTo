#include "LedManager.h"

LedManager::LedManager() :
    Component("leds")
{
    
}

void LedManager::init() {
    rgbManager.init();
}

void LedManager::update() {
    rgbManager.update();
}
