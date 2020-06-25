#include "StreamMode.h"

#ifdef LED_COUNT

StreamMode::StreamMode(CRGB *leds, int numLeds) : LedMode("stream", leds, numLeds),
                                                  //ledBufferIndex(0),
                                                  //colorBufferIndex(0)
                                                  byteIndex(0)
{
    streamBuffer = (uint8_t *)malloc(numLeds * 3 + 1);
}

StreamMode::~StreamMode()
{
    free(streamBuffer);
}

void StreamMode::init()
{
}

void StreamMode::update()
{
    long curTime = millis();
    if (curTime > lastReceiveTime + (1000 / receiveRate))
    {
        receiveUDP();
        lastReceiveTime = curTime;
    }
}

void StreamMode::receiveUDP()
{
    while (int packetSize = udp.parsePacket())
    {
        int numRead = udp.read(streamBuffer, numLeds * 3 + 1);
       
       // DBG("Packet size : " + String(packetSize) + ", numRead : "+String(numRead));
       
       if(numRead == 0) return;
        bool isFinal = streamBuffer[numRead - 1] == 255;
        if(isFinal) numRead--;

        if(byteIndex+numRead > numLeds * 3)
        {
            DBG("Stream OVERFLOW, end index would reach " +String(byteIndex+numRead));
        }else
        {
           // DBG(" > Copying at "+String(byteIndex));
            memcpy((uint8_t *)leds+byteIndex, streamBuffer, numRead);
        }
        
        byteIndex += numRead;
        if (isFinal)
        {
           // DBG(" > isFinal : " +String((int)isFinal));
            byteIndex = 0;
            //processBuffer();
        }
        else
        {
            /*
            for (int i = 0; i < numRead - 1; i++)
            {
                colorBuffer[colorBufferIndex] = streamBuffer[i];
                colorBufferIndex++;
                if (colorBufferIndex == 3)
                {
                    if (ledBufferIndex < numLeds)
                    {
                        leds[LEDMAP(ledBufferIndex)] = CRGB(colorBuffer[0], colorBuffer[1], colorBuffer[2]);
                    }
                    else
                    {
                        DBG("Overflow ! " + String(ledBufferIndex) + ", max leds is " + String(numLeds));
                    }

                    colorBufferIndex = 0;
                    ledBufferIndex++;
                }
            }
            */
        }
    }
}

void StreamMode::processBuffer()
{
    //if (isActive)
    //    memcpy(leds, ledBuffer, numLeds * sizeof(CRGB));
    //memset()
    //colorBufferIndex = 0;
    //ledBufferIndex = 0;
}

void StreamMode::start()
{
    LedMode::start();
    udp.begin(8888);
    udp.flush();
}

void StreamMode::stop()
{
    LedMode::stop();
    udp.flush();
    udp.stop();
}

bool StreamMode::handleCommand(String command, var *data, int numData)
{
    return false;
}

#endif