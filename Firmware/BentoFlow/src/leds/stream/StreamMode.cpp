#include "StreamMode.h"

#ifdef LED_COUNT

StreamMode::StreamMode(CRGB *leds, int numLeds) : LedMode("stream", leds, numLeds),
                                                  //ledBufferIndex(0),
                                                  //colorBufferIndex(0)
                                                  byteIndex(0),
                                                  hasOverflowed(false)
{
    //streamBuffer = (uint8_t *)malloc(MAX_PACKET_SIZE);
}

StreamMode::~StreamMode()
{
   // free(streamBuffer);
}

void StreamMode::init()
{
}

bool StreamMode::update()
{
    long curTime = millis();
    if (curTime > lastReceiveTime + (1000 / receiveRate))
    {
        lastReceiveTime = curTime;
        return receiveUDP();
    }
    return false;
}

bool StreamMode::receiveUDP()
{
    while (int packetSize = udp.parsePacket())
    {
        int numRead = udp.read(streamBuffer, MAX_PACKET_SIZE);
       
        //DBG("Packet size : " + String(packetSize) + ", numRead : "+String(numRead));
       
        if(numRead == 0) return false;
        bool isFinal = streamBuffer[numRead - 1] == 255;

        if(isFinal)
        {
            if(hasOverflowed) //if had overflowed, discard current packet and reset for next one
            {
                DBG("Discard overflowed packet, reset for next one");
                byteIndex = 0;
                hasOverflowed = false;
                return false;
            }

            numRead--;
        } 

        if(byteIndex+numRead > numLeds * 3)
        {
            DBG("Stream OVERFLOW, end index would reach " +String(byteIndex+numRead));
            hasOverflowed = true;
        }else
        {
            //DBG(" > Copying at "+String(byteIndex));
            memcpy((uint8_t *)leds+byteIndex, streamBuffer, numRead);
        }
        
        byteIndex += numRead;
        if (isFinal)
        {
            byteIndex = 0;
            return true;
        }
    }

    return false;
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