bool ButtonComponent::initInternal(JsonObject o)
{
    value.val.type = 'b'; //force boolean
    value.set(false); 

    bool result = IOComponent::initInternal(o);
    
    debounceCount = 0;
    timeAtPress = 0;

    AddParameter(multiPressCount);
    multiPressCount.readOnly = true;
    
    AddParameter(isLongPressed);
    isLongPressed.readOnly = true;

    AddParameter(isVeryLongPressed);
    isVeryLongPressed.readOnly = true;

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
        if (multiPressCount.intValue() > 0)
        {
            multiPressCount.set(0);
            var data[1]{multiPressCount.intValue()};
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
            multiPressCount.set(multiPressCount.intValue() + 1);
            var data[1]{multiPressCount.intValue()};
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
