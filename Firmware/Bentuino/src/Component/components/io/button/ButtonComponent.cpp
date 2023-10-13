ImplementManagerSingleton(Button);

bool ButtonComponent::initInternal(JsonObject o)
{
    // value.val.type = 'b'; //force boolean
    // SetParam(value,false);

    bool result = IOComponent::initInternal(o);

    debounceCount = 0;
    timeAtPress = 0;

    AddIntParam(multiPressCount);
    // multiPressCount.readOnly = true;

    AddIntParam(longPress);
    // longPress.readOnly = true;

    AddIntParam(veryLongPress);
    
    AddBoolParam(canShutDown);
    // veryLongPress.readOnly = true;

    return result;
}

void ButtonComponent::updateInternal()
{
    IOComponent::updateInternal();

    if (value)
    {
        if (!longPress && millis() > timeAtPress + LONGPRESS_TIME)
        {
            SetParam(longPress, true);
        }

        if (!veryLongPress && millis() > timeAtPress + VERYLONGPRESS_TIME)
        {
            SetParam(veryLongPress, true);
        }
    }

    if (millis() > timeAtPress + MULTIPRESS_TIME)
    {
        if (multiPressCount > 0)
        {
            SetParam(multiPressCount, 0);
        }
    }
}

void ButtonComponent::paramValueChangedInternal(void *param)
{
    IOComponent::paramValueChangedInternal(param);

    if (param == &value)
    {
        // NDBG("Reset long press and very long press");
        SetParam(veryLongPress, false);
        SetParam(longPress, false);

        if (value)
        {
            timeAtPress = millis();
            SetParam(multiPressCount, multiPressCount + 1);
        }
        else
        {
            if (millis() < timeAtPress + SHORTPRESS_TIME)
            {
                // sendEvent(ShortPress); //should be trigger
            }
        }
    }
}
