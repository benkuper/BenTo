#include "LedManager.h"
#ifdef LED_COUNT
#include "../MainManager.h"

LedManager::LedManager() : Component("leds"),
                           mode(Mode::Direct),
                           currentMode(nullptr),
                           sysLedMode(rgbManager.leds[LED_BASE_LAYER], LED_COUNT),
                           streamMode(rgbManager.leds[LED_BASE_LAYER], LED_COUNT),
                           playerMode(rgbManager.leds[LED_BASE_LAYER], LED_COUNT),
#ifdef HAS_LED_FX
                           fxManager(rgbManager.leds[LED_SCRIPT_LAYER], rgbManager.leds[LED_OUT_LAYER]),
#endif
                           connectedTimer(2000),
                           connectionIsAlive(false)
{

    playerMode.addListener(std::bind(&LedManager::playerEvent, this, std::placeholders::_1));
    connectedTimer.addListener(std::bind(&LedManager::timerEvent, this, std::placeholders::_1));
}

void LedManager::init()
{

#ifdef GENERATE_LED_INDEX_MAP
    generateLedIndexMap();
#endif

    rgbManager.init();
    rgbManager.addListener(std::bind(&LedManager::rgbLedsEvent, this, std::placeholders::_1));
    sysLedMode.init();

#ifdef HAS_IR
    irManager.init();
#endif

    setMode(System);
}

void LedManager::update()
{
    // if (mode == System) rgbManager.clear();

    bool shouldUpdateLeds = true;

    if (connectionIsAlive || playerMode.isPlaying)
    {
        if (currentMode != nullptr)
            shouldUpdateLeds = currentMode->update();
    }
    else
    {
        rgbManager.point(CHSV(0, 255, cos(millis() / 200.0f) * 127 + 127), .5f, .05f);
        shouldUpdateLeds = true;
    }

    memcpy(rgbManager.leds[LED_SCRIPT_LAYER], rgbManager.leds[LED_BASE_LAYER], LED_COUNT * sizeof(CRGB));
#ifdef HAS_SCRIPTS
    MainManager::instance->scripts.update();
#endif
    memcpy(rgbManager.leds[LED_OUT_LAYER], rgbManager.leds[LED_SCRIPT_LAYER], LED_COUNT * sizeof(CRGB));

#ifdef HAS_LED_FX
    fxManager.update();
#endif

    rgbManager.update();

#ifdef HAS_IR
    irManager.update();
#endif

    connectedTimer.update();
}

void LedManager::showBaseOnOutLayer()
{
    memcpy(rgbManager.leds[LED_OUT_LAYER], rgbManager.leds[LED_BASE_LAYER], LED_COUNT * sizeof(CRGB));
    FastLED.show();
}

void LedManager::setMode(Mode m)
{
    NDBG("LED Set mode " + String((int)m));

    if (m == mode)
        return;

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
#ifndef NO_ANIMATIONS

    CRGB initLeds[LED_COUNT];
    memcpy(initLeds, rgbManager.leds[LED_OUT_LAYER], LED_COUNT * sizeof(CRGB));

    rgbManager.setLedEnabled(true);
    for (int i = 0; i < 255; i++)
    {
        for (int led = 0; led < LED_COUNT; led++)
            rgbManager.setLed(led, blend(initLeds[led], color, i), LED_OUT_LAYER);

        rgbManager.update();
        FastLED.delay(1);
    }

    FastLED.delay(50);

    for (float i = 0; i <= 1; i += .01f)
    {
        rgbManager.fillRange(color, i, 1, true, LED_OUT_LAYER);
        rgbManager.update();
        FastLED.delay(2);
    }

    rgbManager.clear();
    FastLED.delay(30);
    rgbManager.update();
    rgbManager.shutdown();

#endif // NO_ANIMATIONS

    delay(100);
}

void LedManager::setConnectionState(ConnectionState state)
{
    if (state == PingDead || state == PingAlive || state == Connected || state == Connecting)
    {
        connectionIsAlive = state == PingAlive || state == Connected || state == Connecting;
        rgbManager.clear();
        if (state == PingAlive || state == PingDead)
            return;
    }

    setMode(System);
    sysLedMode.setConnectionState(state);

    if (state == Connected || state == Hotspot)
        connectedTimer.start();
    else
        connectedTimer.stop();
}

void LedManager::rgbLedsEvent(const RGBLedsEvent &e)
{
    if (e.type == RGBLedsEvent::ASK_FOCUS)
        setMode(Direct);
}

void LedManager::playerEvent(const PlayerEvent &e)
{
    if (e.type == PlayerEvent::Load || e.type == PlayerEvent::Play)
    {
        setMode(Player);
    }
}

void LedManager::timerEvent(const TimerEvent &e)
{
    if (e.timer == &connectedTimer)
    {
        setMode(Stream);
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

        return true;
    }
    else if (checkCommand(command, "playAndIr", numData, 2))
    {
        if (data[0].type == 'f')
        {
            playerMode.play(data[0].floatValue());
        }
        if (data[1].type == 'f')
        {
#ifdef HAS_IR
            irManager.setBrightness(data[1].floatValue());
#endif
        }

        return true;
    }

    return false;
}

#endif