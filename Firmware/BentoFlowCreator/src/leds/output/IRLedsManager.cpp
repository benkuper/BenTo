#include "IRLedsManager.h"

IRLedsManager::IRLedsManager() : Component("ir"), brightness(0) {
    
}

IRLedsManager::~IRLedsManager() {
    
}

void IRLedsManager::init() {
    ledcSetup(IR_CHANNEL, IR_FREQ, IR_PWM_RESOLUTION);
    ledcAttachPin(IR_TX_PIN, IR_CHANNEL);
    ledcWrite(IR_CHANNEL, 0);
}

void IRLedsManager::update() {

}

void IRLedsManager::setBrightness(float value) {
    if(brightness == value) return;
    brightness = min(max(value,0.f),1.f);
    ledcWrite(IR_CHANNEL, (int)(brightness * IR_MAX_POWER));
}

bool IRLedsManager::handleCommand(String command, var * data, int numData) {
    NDBG("Handle command : "+command);
    if (checkCommand(command, "brightness", numData, 1))
    {
        setBrightness(data[0].floatValue());
        return true;
    }

    return false;
}
