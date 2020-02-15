#pragma once

#if SERIAL_DEBUG
#define DBG(msg) Serial.println(msg)
#else
#define DBG(msg)
#endif

class SerialManager
{
  public:
    SerialManager()
    {
      instance = this;
      addCallbackMessageReceived(&SerialManager::defaultCallback);

    }

    ~SerialManager() {}

    static SerialManager * instance;
    char buffer[256];
    int bufferIndex = 0;

    void init()
    {
      Serial.begin(115200);
      memset(buffer, 0, 32);
    }

    void update()
    {
      while (Serial.available())
      {
        byte c = Serial.read();
        //DBG("Got char : "+String(c));
        if (c == 255 || c == '\n')
        {
          onMessageReceived(buffer);
          memset(buffer, 0, 255);
          bufferIndex = 0;
        } else
        {
          if (bufferIndex < 255) buffer[bufferIndex] = c;
          bufferIndex++;
        }
      }
    }

    int splitString(char * source, String * dest, int maxNum)
    {
      char * pch = strtok (source, ",");
      int i = 0;
      while (pch != NULL && i < maxNum)
      {
        dest[i] = String(pch);
        pch = strtok (NULL, ",");
        i++;
      }

      return i;
    }

    void sendTrigger(String name)
    {
      Serial.println(name);
    }

    void sendBoolValue(String name, bool value)
    {
      Serial.println(name + " " + (value ? 1 : 0));
    }

    void sendIntValue(String name, int value)
    {
      Serial.println(name + " " + String(value));
    }

    void sendFloatValue(String name, float value)
    {
      Serial.println(name + " " + String(value));
    }

      typedef void(*onEvent)(String message);
    void (*onMessageReceived) (String);
    
    void addCallbackMessageReceived (onEvent func) {
      onMessageReceived = func;
    }

     static void defaultCallback(String message){}
};

SerialManager * SerialManager::instance = nullptr;
