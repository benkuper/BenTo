#pragma once

#include "../../common/Common.h"

#ifdef HAS_DISPLAY
#include <M5StickCPlus.h>
#define DISPLAY_LINES 6
#define DISPLAY_REFRESH_RATE 10

class ButtonManager;
class IMUManager;
class WifiManager;

class DisplayManager : public Component
{
public:
    DisplayManager();
    ~DisplayManager() {}

    bool outputEnabled;

    ButtonManager * buttons;
    WifiManager * wifi;
    IMUManager * imu;

    String messages[DISPLAY_LINES];
    int messageIndex;

    long timeAtLastUpdate;

    //SSD1306Wire display; //Set OLED to default address 0x3C, and pins 4 and 15, with reset at pin 16

    void init();
    void update();
    void updateInternal();

    void sendMessage(String source, String command, var * data, int numData);

    void logMessage(String msg);

    bool handleCommand(String command, var * data, int numData) override;
};

#endif