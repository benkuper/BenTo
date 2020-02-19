
#include "ButtonManager.h"

const String ButtonEvent::eventNames[ButtonEvent::TYPES_MAX] {"pressed","released","shortPress","longPress","multiPress" };

ButtonManager::ButtonManager() : Component("button"),
                                 isPressed(false),
                                 debounceCount(0),
                                 multiPressCount(0),
                                 timeAtPress(0)
{
}

void ButtonManager::init()
{
    pinMode(BUTTON_PIN, INPUT);
    isPressed = digitalRead(BUTTON_PIN); //init here so it won't send an event at first update
}

void ButtonManager::update()
{
    bool v = digitalRead(BUTTON_PIN);

    if (v)
        debounceCount = min(debounceCount + 1, buttonPressDebounce);
    else
        debounceCount = max(debounceCount - 1, 0);

    bool newPressed = isPressed;
    if (isPressed && debounceCount == 0)
        newPressed = false;
    if (!isPressed && debounceCount == buttonPressDebounce)
        newPressed = true;

    if (newPressed != isPressed) //button state changed
    {
        isPressed = newPressed;
        isLongPressed = false;
        isVeryLongPressed = false;

        if (isPressed)
        {
            timeAtPress = millis();
            sendEvent(ButtonEvent(ButtonEvent::Pressed));

            multiPressCount++;
            if (multiPressCount > 1)
                sendEvent(ButtonEvent(ButtonEvent::MultiPress, multiPressCount));
        }
        else
        {
            if (millis() < timeAtPress + shortPressTime)
            {
                sendEvent(ButtonEvent(ButtonEvent::ShortPress));
            }

            sendEvent(ButtonEvent(ButtonEvent::Released));
        }
    }
    else
    {
        if (isPressed)
        {
            if (!isLongPressed && millis() > timeAtPress + longPressTime)
            {
                isLongPressed = true;
                sendEvent(ButtonEvent(ButtonEvent::LongPress));
            }

            if (!isVeryLongPressed && millis() > timeAtPress + veryLongPressTime)
            {
                isVeryLongPressed = true;
                sendEvent(ButtonEvent(ButtonEvent::VeryLongPress));
            }
        }

        if (millis() > timeAtPress + multiPressTime)
            multiPressCount = 0;
    }
}
