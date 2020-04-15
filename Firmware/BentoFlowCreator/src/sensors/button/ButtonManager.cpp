
#include "ButtonManager.h"

const String ButtonEvent::eventNames[ButtonEvent::TYPES_MAX]{"pressed", "shortPress", "longPress", "veryLongPress", "multiPress"};

ButtonManager::ButtonManager() : Component("button")
{
    for (int i = 0; i < BUTTON_COUNT; i++)
    {
        isPressed[i] = false;
        debounceCount[i] = 0;
        multiPressCount[i] = 0;
        timeAtPress[i] = 0;
    }
}

void ButtonManager::init()
{
#if BUTTON_COUNT > 0
    for (int i = 0; i < BUTTON_COUNT; i++)
    {
        pinMode(buttonPins[i], BUTTON_INPUT_MODE);
        isPressed[i] = digitalRead(buttonPins[i]);
#ifdef BUTTON_INVERTED
        isPressed[i] = !isPressed[i];
        NDBG("Button "+String(i)+" init : "+isPressed[i]);
#endif
    }
#endif //BUTTON_COUNT > 0
}

void ButtonManager::update()
{
#if BUTTON_COUNT > 0
    for (int i = 0; i < BUTTON_COUNT; i++)
    {
        bool v = digitalRead(buttonPins[i]);
#ifdef BUTTON_INVERTED
        v = !v;
#endif
        if (v)
            debounceCount[i] = min(debounceCount[i] + 1, buttonPressDebounce);
        else
            debounceCount[i] = max(debounceCount[i] - 1, 0);

        bool newPressed = isPressed[i];
        if (isPressed[i] && debounceCount[i] == 0)
            newPressed = false;
        if (!isPressed[i] && debounceCount[i] == buttonPressDebounce)
            newPressed = true;

        if (newPressed != isPressed[i]) //button state changed
        {
            isPressed[i] = newPressed;
            isLongPressed[i] = false;
            isVeryLongPressed[i] = false;

            if (isPressed[i])
            {
                timeAtPress[i] = millis();
                sendEvent(ButtonEvent(ButtonEvent::Pressed, i, 1));

                multiPressCount[i]++;
                sendEvent(ButtonEvent(ButtonEvent::MultiPress, i, multiPressCount[i]));
            }
            else
            {
                if (millis() < timeAtPress[i] + shortPressTime)
                {
                    sendEvent(ButtonEvent(ButtonEvent::ShortPress, i));
                }

                sendEvent(ButtonEvent(ButtonEvent::Pressed, i, 0));
            }
        }
        else
        {
            if (isPressed[i])
            {
                if (!isLongPressed[i] && millis() > timeAtPress[i] + longPressTime)
                {
                    isLongPressed[i] = true;
                    sendEvent(ButtonEvent(ButtonEvent::LongPress, i));
                }

                if (!isVeryLongPressed[i] && millis() > timeAtPress[i] + veryLongPressTime)
                {
                    isVeryLongPressed[i] = true;
                    sendEvent(ButtonEvent(ButtonEvent::VeryLongPress, i));
                }
            }

            if (millis() > timeAtPress[i] + multiPressTime)
            {
                if(multiPressCount[i] > 0)
                {
                    multiPressCount[i] = 0;
                    sendEvent(ButtonEvent(ButtonEvent::MultiPress, i, multiPressCount[i]));
                }
            }
        }
    }
#endif //MULTI BUTTON
}
