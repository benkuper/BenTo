#include "LedManager.h"

LedManager::LedManager() : 
    Component("leds"),
    sysLedMode(rgbManager.leds, LEDS_COUNT)
{
}

void LedManager::init()
{
    rgbManager.init();
    sysLedMode.init();

    setMode(System);
}

void LedManager::update()
{
    rgbManager.clear();

    if(currentMode != nullptr) currentMode->update();

    rgbManager.update();
}

void LedManager::setMode(Mode m)
{
    mode = m;
    switch (mode)
    {
    case Direct:
        currentMode = nullptr;
        break;

    case System:
        currentMode = &sysLedMode;
        break;
    //case Stream: currentMode = &streamMode; break;
    //case Player: currentMode = &playerMode; break;
    default:
        break;
    }
}

void LedManager::wakeup(CRGB color)
{
    for (float i = 0; i < 1; i += .01f)
    {
        rgbManager.fillRange(color, 0, i, true);
        rgbManager.update();
        delay(5);
    }

    delay(100);
}

void LedManager::shutdown(CRGB color)
{
    for (float i = 1; i >= 0; i -= .01f)
    {
        rgbManager.fillRange(color, 0, i, true);
        rgbManager.update();
        delay(5);
    }

    rgbManager.clear();
    rgbManager.update();
    delay(100);
}

bool LedManager::handleCommand(String command, var *data, int numData) 
{
    if(checkCommand(command,"mode",numData,1))
    {
        if(data[0].type == 's')
        {
            if(data[0].stringValue() == "direct") setMode(Direct);
            else if(data[0].stringValue() == "system") setMode(System);
            else if(data[0].stringValue() == "stream") setMode(Stream);
            else if(data[0].stringValue() == "player") setMode(Player);
        }else
        {
            setMode((Mode)data[0].intValue());
        }
    }    
}
