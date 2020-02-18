#include "StreamMode.h"

StreamMode::StreamMode(CRGB *leds, int numLeds) : 
LedMode("stream", leds, numLeds),
    ledBufferIndex(0),
    colorBufferIndex(0)
{
    ledBuffer = (CRGB *)malloc(numLeds * sizeof(CRGB));
}

StreamMode::~StreamMode() {
    free(ledBuffer);
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
    if (udp.parsePacket())
    {
        while (udp.available() > 0)
        {
            byte b = udp.read();

            if (b == 255)
            {
                processBuffer();
                udp.flush();
            }
            else
            {
                colorBuffer[colorBufferIndex] = (uint8_t)b;
                colorBufferIndex++;
                if (colorBufferIndex == 3)
                {
                    if (ledBufferIndex < numLeds)
                    {
                        ledBuffer[ledBufferIndex] = CRGB(colorBuffer[0], colorBuffer[1], colorBuffer[2]);
                    }
                    else
                    {
                        DBG("Overflow ! " + String(ledBufferIndex));
                    }

                    //Serial.print(leds[ledBufferIndex],DEC);
                    //Serial.print(" ");
                    colorBufferIndex = 0;
                    ledBufferIndex++;
                }
            }
        }
    }
}


void StreamMode::processBuffer()
{
    if(isActive) memcpy(leds, ledBuffer, numLeds * sizeof(CRGB));
    colorBufferIndex = 0;
    ledBufferIndex = 0;
}

void StreamMode::start()
{
    LedMode::start();

    DBG("Start receiving on port 8888");
    udp.begin(8888);
    udp.flush();
}

void StreamMode::stop()
{
    LedMode::stop();

    DBG("Stop receiving");
    udp.flush();
    udp.stop();
}


bool StreamMode::handleCommand(String command, var *data, int numData)
{
    return false;
}

