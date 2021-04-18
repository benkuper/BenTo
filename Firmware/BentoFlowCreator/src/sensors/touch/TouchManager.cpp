
#include "TouchManager.h"

const String TouchEvent::eventNames[TouchEvent::TYPES_MAX]{"pressed", "shortPress", "longPress", "veryLongPress", "multiPress"};

TouchManager::TouchManager() : 
 Component("touch"),
 touchThreshold(90)
{
    for (int i = 0; i < TOUCH_COUNT; i++)
    {
        isPressed[i] = false;
        debounceCount[i] = 0;
        multiPressCount[i] = 0;
        timeAtPress[i] = 0;
    }
}

void TouchManager::init()
{

#if TOUCH_COUNT > 0
    for (int i = 0; i < TOUCH_COUNT; i++)
    {
        
        //pinMode(buttonPins[i], BUTTON_INPUT_MODE);
        //isPressed[i] = digitalRead(buttonPins[i]);
    }
#endif //TOUCH_COUNT > 0

#ifdef TOUCH_WAKEUP_PIN
//Setup interrupt on Touch Pad 3 (GPIO15)
 // touchAttachInterrupt(TOUCH_WAKEUP_PIN, touchCallback, touchThreshold);
#endif //TOUCH_WAKEUP_PIN

}

void TouchManager::update()
{
#if TOUCH_COUNT > 0
    for (int i = 0; i < TOUCH_COUNT; i++)
    {
        bool v = touchRead(touchPins[i]) < touchThreshold;

        if (v)
            debounceCount[i] = min(debounceCount[i] + 1, touchPressDebounce);
        else
            debounceCount[i] = max(debounceCount[i] - 1, 0);

        bool newPressed = isPressed[i];
        if (isPressed[i] && debounceCount[i] == 0)
            newPressed = false;

        if (!isPressed[i] && debounceCount[i] == touchPressDebounce)
            newPressed = true;

        if (newPressed != isPressed[i]) //touch state changed
        {
            isPressed[i] = newPressed;
            isLongPressed[i] = false;
            isVeryLongPressed[i] = false;

            if (isPressed[i])
            {
                timeAtPress[i] = millis();
                sendEvent(TouchEvent(TouchEvent::Pressed, i, 1));

                multiPressCount[i]++;
                sendEvent(TouchEvent(TouchEvent::MultiPress, i, multiPressCount[i]));
            }
            else
            {
                if (millis() < timeAtPress[i] + shortPressTime)
                {
                    sendEvent(TouchEvent(TouchEvent::ShortPress, i));
                }

                sendEvent(TouchEvent(TouchEvent::Pressed, i, 0));
            }
        }
        else
        {
            if (isPressed[i])
            {
                if (!isLongPressed[i] && millis() > timeAtPress[i] + longPressTime)
                {
                    isLongPressed[i] = true;
                    sendEvent(TouchEvent(TouchEvent::LongPress, i));
                }

                if (!isVeryLongPressed[i] && millis() > timeAtPress[i] + veryLongPressTime)
                {
                    isVeryLongPressed[i] = true;
                    sendEvent(TouchEvent(TouchEvent::VeryLongPress, i));
                }
            }

            if (millis() > timeAtPress[i] + multiPressTime)
            {
                if(multiPressCount[i] > 0)
                {
                    multiPressCount[i] = 0;
                    sendEvent(TouchEvent(TouchEvent::MultiPress, i, multiPressCount[i]));
                }
            }
        }
    }
#endif //TOUCH_COUNT
}

void TouchManager::touchCallback()
{
    DBG("Touch callback");
}

void TouchManager::setTouchThreshold(int value)
{
    NDBG("Touch threshold is now "+String(value));
    touchThreshold = value;
   // touch_pad_set_thresh(TOUCH_WAKEUP_PIN, value);
}

bool TouchManager::handleCommand(String command, var *data, int numData)
{
#ifdef TOUCH_COUNT
    if (checkCommand(command, "threshold", numData, 0))
    {
        setTouchThreshold(data[0].intValue());
        return true;
    }
#endif
    return false;
}