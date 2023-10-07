#include "CapacitiveManager.h"

#ifdef HAS_CAPA
const String CapacitiveEvent::eventNames[CapacitiveEvent::TYPES_MAX]{"touch", "values"};

CapacitiveManager::CapacitiveManager() : Component("capacitive"),
                                         isConnected(false),
                                         isEnabled(true),
                                         sendLevel(2),
                                         capSendTime(20), //50fps
                                         timeSinceCapLastSent(0),
                                         lastTouchStates(0)
{
}

CapacitiveManager::~CapacitiveManager()
{
}

void CapacitiveManager::init()
{
#ifdef CAPACITIVE_COUNT
    NDBG("Init");
    if (isConnected)
        return;

    if (!cap.begin(0x5A))
    {
        Serial.println("MPR121 not found, check wiring?");
        isConnected = false;
        return;
    }

    isConnected = true;
    NDBG("Capacitive is connected.");

#ifdef USE_PREFERENCES
//prefs.begin(name.c_str());
//prefs.end();
#endif

#endif
}

void CapacitiveManager::update()
{
    if (!isEnabled || !isConnected)
        return;

#ifdef CAPACITIVE_COUNT

    uint16_t touchStates = cap.touched();

    for (int i = 0; i < CAPACITIVE_COUNT; i++)
    {
        bool t = touchStates & (1 << i);
        setTouch(i, t);
        values[i] = cap.filteredData(i);
    }

    long curTime = millis();
    if (curTime > timeSinceCapLastSent + capSendTime)
    {
        if (sendLevel >= 2)
        {

            sendEvent(CapacitiveEvent(CapacitiveEvent::ValuesUpdate, values, CAPACITIVE_COUNT));
        }

        timeSinceCapLastSent = curTime;
    }
#endif
}
void CapacitiveManager::setTouch(int index, bool value)
{
#ifdef CAPACITIVE_COUNT
    if (touches[index] == value)
        return;
    touches[index] = value;
    sendEvent(CapacitiveEvent(CapacitiveEvent::TouchUpdate, index, value));
#endif
}

void CapacitiveManager::setEnabled(bool value)
{
#ifdef CAPACITIVE_COUNT
    if (isEnabled == value)
        return;
    isEnabled = value;
#endif
}

bool CapacitiveManager::handleCommand(String command, var *data, int numData)
{
#ifdef CAPACITIVE_COUNT
    if (checkCommand(command, "enabled", numData, 1))
    {
        setEnabled(data[0].intValue());
        return true;
    }
    if (checkCommand(command, "sendLevel", numData, 1))
    {
        sendLevel = data[0].intValue();
        return true;
    }
    else if (checkCommand(command, "updateRate", numData, 1))
    {
        capSendTime = 1000 / data[0].intValue();
        return true;
    }
#endif
    return false;
}

#endif