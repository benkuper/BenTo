#include "SerialManager.h"

SerialManager::SerialManager() :
    Component("serial")
{
    memset(buffer, 0, 512);
}

void SerialManager::init()
{
    Serial.begin(115200);
}

void SerialManager::update()
{
    while (Serial.available())
    {
        char c = Serial.read();
        if (c == '\n')
        {
            processMessage(buffer);
            memset(buffer, 0, 512);
            bufferIndex = 0;
        }else
        {
            if(bufferIndex < 512) buffer[bufferIndex] = c;
            bufferIndex++;
        }
        
    }
}

void SerialManager::processMessage(String buffer)
{
    int splitIndex = buffer.indexOf(' ');
    
    String tc = buffer.substring(0, splitIndex);
    int tcIndex = tc.indexOf('.');

    sendEvent(SerialEvent(SerialEvent::MessageReceived, tc.substring(0,tcIndex), tc.substring(tcIndex+1), buffer.substring(splitIndex + 1)));
}
