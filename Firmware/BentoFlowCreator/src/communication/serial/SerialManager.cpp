#include "SerialManager.h"

SerialManager::SerialManager() :
    Component("serial")
{
    memset(buffer, 0, 64);
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
            memset(buffer, 0, 64);
            bufferIndex = 0;
        }else
        {
            if(bufferIndex < 64) buffer[bufferIndex] = c;
            bufferIndex++;
        }
        
    }
}

void SerialManager::processMessage(String buffer)
{
    int splitIndex = buffer.indexOf(' ');
    sendEvent(SerialEvent(SerialEvent::MessageReceived, buffer.substring(0, splitIndex), buffer.substring(splitIndex + 1, buffer.length() - 1)));
}
