bool ButtonComponent::initInternal(JsonObject o)
{
    bool result = IOComponent::initInternal(o);

    debounceCount = 0;
    timeAtPress = 0;

    AddAndSetParameter(multiPressCount);
    AddAndSetParameter(isLongPressed);
    AddAndSetParameter(isVeryLongPressed);

    return result;
}

void ButtonComponent::updateInternal()
{
    IOComponent::updateInternal();

    if (value.boolValue())
    {
        if (!isLongPressed.boolValue() && millis() > timeAtPress + LONGPRESS_TIME)
        {
            isLongPressed.set(true);
            sendEvent(LongPress);
        }

        if (!isVeryLongPressed.boolValue() && millis() > timeAtPress + VERYLONGPRESS_TIME)
        {
            isVeryLongPressed.set(true);
            sendEvent(VeryLongPress);
        }
    }

    if (millis() > timeAtPress + MULTIPRESS_TIME)
    {
        if (multiPressCount.boolValue() > 0)
        {
            multiPressCount.set(0);
            var data[1]{multiPressCount.boolValue()};
            sendEvent(MultiPress, data, 1);
        }
    }
}

void ButtonComponent::onParameterEventInternal(const ParameterEvent &e)
{
    IOComponent::onParameterEventInternal(e);

    if (e.parameter == &value)
    {
        isLongPressed.set(false);
        isVeryLongPressed.set(false);

        if (value.boolValue())
        {
            timeAtPress = millis();
            multiPressCount.set(multiPressCount.boolValue() + 1);
            var data[1]{multiPressCount.boolValue()};
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
