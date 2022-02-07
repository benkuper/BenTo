bool ButtonComponent::initInternal()
{
    bool result = IOComponent::initInternal();

    isSystem = GetBoolConfig("isSystem");
    
    debounceCount = 0;
    multiPressCount = 0;
    timeAtPress = 0;
    isLongPressed = false;
    isVeryLongPressed = false;

    return result;
}

void ButtonComponent::updateInternal()
{
    IOComponent::updateInternal();

    if (value->boolValue())
    {
        if (!isLongPressed && millis() > timeAtPress + LONGPRESS_TIME)
        {
            isLongPressed = true;
            sendEvent(LongPress);
        }

        if (!isVeryLongPressed && millis() > timeAtPress + VERYLONGPRESS_TIME)
        {
            isVeryLongPressed = true;
            sendEvent(VeryLongPress);
        }
    }

    if (millis() > timeAtPress + MULTIPRESS_TIME)
    {
        if (multiPressCount > 0)
        {
            multiPressCount = 0;
            var data[1]{multiPressCount};
            sendEvent(MultiPress, data, 1);
        }
    }
}

void ButtonComponent::onParameterEvent(const ParameterEvent &e)
{
    IOComponent::onParameterEvent(e);

    if (e.parameter == value)
    {
        isLongPressed = false;
        isVeryLongPressed = false;

        if (value->boolValue())
        {
            timeAtPress = millis();
            multiPressCount++;
            var data[1]{multiPressCount};
            sendEvent(MultiPress, data, 1);
        }
        else
        {
            if (millis() < timeAtPress + SHORTPRESS_TIME)
            {
                sendEvent(ShortPress);
            }
        }
    }
}
