#include "SerialManager.h"

SerialManager::SerialManager() :
    Component("serial"),
    outputEnabled(false)
{
    memset(buffer, 0, 512);
}

void SerialManager::init()
{
    Serial.begin(115200);
    NDBG("Init");
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
    if(buffer.substring(0,2) == "yo")
    {
         Serial.println("wassup "+getDeviceID()+" \""+String(DEVICE_TYPE)+"\"");
        return;
    }

    int splitIndex = buffer.indexOf(' ');
    
    String tc = buffer.substring(0, splitIndex);
    int tcIndex = tc.indexOf('.');
    String args = splitIndex != -1 ?buffer.substring(splitIndex + 1):"";

    sendEvent(SerialEvent(SerialEvent::MessageReceived, tc.substring(0,tcIndex), tc.substring(tcIndex+1), args));
}


void SerialManager::sendMessage(String source, String command, var * data, int numData)
{
    if(!outputEnabled) return;

    String msg = source+"."+command;
    for(int i=0;i<numData;i++)
    {
        msg += " "+data[i].stringValue();
    }

    Serial.println(msg);
}

bool SerialManager::handleCommand(String command, var * data, int numData)
{
    if(checkCommand(command,"outputEnabled", numData, 1))
    {
        outputEnabled = data[0].intValue() == 1;
        return true;
    }

    return false;
}
