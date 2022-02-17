#include "DisplayManager.h"

#ifdef HAS_DISPLAY

#include "../../sensors/button/ButtonManager.h"
#include "../../sensors/imu/IMUManager.h"
#include "../wifi/WifiManager.h"
#include "../../common/DebugHelpers.h"

DisplayManager::DisplayManager() : Component("display"),
                                   outputEnabled(true),
                                   display(0x3C, DISPLAY_SDA, DISPLAY_SCL),
                                   messageIndex(0),
                                   timeAtLastUpdate(0)
{
    for (int i = 0; i < DISPLAY_LINES; i++)
        messages[i] = "";
}

void DisplayManager::init()
{
    buttons = (ButtonManager *)Component::getComponentForName("buttons");
    wifi = (WifiManager *)Component::getComponentForName("wifi");
    imu = (IMUManager *)Component::getComponentForName("imu");

    pinMode(DISPLAY_RESET, OUTPUT);
    digitalWrite(DISPLAY_RESET, LOW); // set GPIO16 low to reset OLED
    delay(50);
    digitalWrite(DISPLAY_RESET, HIGH);
    delay(50);

    display.init();
    display.flipScreenVertically();
    display.setFont(ArialMT_Plain_10);
    delay(50);

    NDBG("Init Display");
}

void DisplayManager::update()
{
    if (!outputEnabled)
        return;

    long time = millis();
    if (time > timeAtLastUpdate + 1000 / DISPLAY_REFRESH_RATE)
    {
        updateInternal();
        timeAtLastUpdate = time;
    }
}

void DisplayManager::updateInternal()
{
    display.clear();
    String ws = connectionStateNames[wifi->state];
    if(wifi->state == ConnectionState::Connected) ws += " "+wifi->getIP();

    display.setTextAlignment(TEXT_ALIGN_RIGHT);
    display.drawString(128, 50, ws);

#ifdef BUTTON_COUNT
    for (int i = 0; i < BUTTON_COUNT; i++)
    {
        display.setColor(WHITE);
        display.drawCircle(6+i*12, 6, 4);
        if(buttons->isPressed[i]) display.fillCircle(6+i*12, 6, 2);
    }
#endif

    display.display();
}

void DisplayManager::sendMessage(String source, String command, var *data, int numData)
{
    if (!outputEnabled)
        return;

    String msg = source + "." + command;
    for (int i = 0; i < numData; i++)
        msg += " " + data[i].stringValue();


    logMessage(msg);
}

void DisplayManager::logMessage(String msg)
{
    messageIndex = (messageIndex + 1) % DISPLAY_LINES;
    messages[messageIndex] = msg;

    display.clear();
    for (int i = 0; i < DISPLAY_LINES; i++)
    {
        int line = (i + messageIndex) % DISPLAY_LINES;
        display.drawString(0, line * 15, messages[line]);
    }

    display.display();
}

bool DisplayManager::handleCommand(String command, var *data, int numData)
{
    if (checkCommand(command, "outputEnabled", numData, 1))
    {
        outputEnabled = data[0].intValue() == 1;
        return true;
    }

    return false;
}

#endif