#include "LedManager.h"

LedManager::LedManager() : Component("leds"),
                           mode(Mode::Direct),
                           currentMode(nullptr),
                           sysLedMode(rgbManager.leds, LED_COUNT),
                           streamMode(rgbManager.leds, LED_COUNT),
                           playerMode(rgbManager.leds, LED_COUNT),
                           connectedTimer(2000)
{
    playerMode.addListener(std::bind(&LedManager::playerEvent, this, std::placeholders::_1));
    connectedTimer.addListener(std::bind(&LedManager::timerEvent, this, std::placeholders::_1));
}

void LedManager::init()
{
    rgbManager.init();
    sysLedMode.init();

    setMode(System);
}

void LedManager::update()
{
    if (mode == System)
        rgbManager.clear();
    
    if (currentMode != nullptr)
        currentMode->update();

    rgbManager.update();

    connectedTimer.update();
}

void LedManager::setMode(Mode m)
{
    if (m == mode)
        return;

    DBG("Set mode "+String(m));

    if (currentMode != nullptr)
    {
        currentMode->stop();
    }

    mode = m;
    
    switch (mode)
    {
    case Direct:
        currentMode = nullptr;
        break;

    case System:
        currentMode = &sysLedMode;
        break;
    case Stream:
        currentMode = &streamMode;
        break;

    case Player: 
        currentMode = &playerMode;
        break;

    default:
        break;
    }
    
    rgbManager.clear();

    if (currentMode != nullptr)
    {
        currentMode->start();
    }
}

void LedManager::shutdown(CRGB color)
{
    CRGB initLeds[LED_COUNT];
    memcpy(initLeds, rgbManager.leds, LED_COUNT * sizeof(CRGB));

    for (int i = 0; i < 255; i++)
    {
        for (int led = 0; led < LED_COUNT; led++)
            rgbManager.setLed(led, blend(initLeds[led], color, i));
        FastLED.delay(2);
    }

    FastLED.delay(50);

    for (float i = 1; i >= 0; i -= .01f)
    {
        rgbManager.fillRange(color, 0, i, true);
        rgbManager.update();
        FastLED.delay(3);
    }

    rgbManager.clear();
    rgbManager.update();

    delay(100);
}

void LedManager::setConnectionState(ConnectionState state)
{
    setMode(System);
    sysLedMode.setConnectionState(state);

    if (state == Connected || state == Hotspot)
        connectedTimer.start();
    else
        connectedTimer.stop();
}


void LedManager::playerEvent(const PlayerEvent &e) 
{
    if(e.type == PlayerEvent::Load || e.type == PlayerEvent::Play)
    {
        setMode(Player);
    }    
}


bool LedManager::handleCommand(String command, var *data, int numData)
{
    if (checkCommand(command, "mode", numData, 1))
    {
        if (data[0].type == 's')
        {
            if (data[0].stringValue() == "direct")
                setMode(Direct);
            else if (data[0].stringValue() == "system")
                setMode(System);
            else if (data[0].stringValue() == "stream")
                setMode(Stream);
            else if (data[0].stringValue() == "player")
                setMode(Player);
        }
        else
        {
            setMode((Mode)data[0].intValue());
        }
    }
}

void LedManager::timerEvent(const TimerEvent &e)
{
    if (e.timer == &connectedTimer)
        setMode(Stream);
}
