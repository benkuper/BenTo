ImplementSingleton(BatteryComponent);

bool BatteryComponent::initInternal(JsonObject o)
{
    AddAndSetParameter(batteryPin);
    AddAndSetParameter(chargePin);
    AddAndSetParameter(rawMin);
    AddAndSetParameter(rawMax);
    AddAndSetParameter(sendFeedback);

    AddParameter(batteryLevel);
    AddParameter(isCharging);

    if (batteryPin.intValue() > 0)
    {
        pinMode(batteryPin.intValue(), INPUT);
        analogSetPinAttenuation(batteryPin.intValue(), ADC_0db);
    }

    if (chargePin.intValue() > 0)
        pinMode(chargePin.intValue(), INPUT);

    for (int i = 0; i < VALUES_SIZE; i++)
        values[i] = 0;

    return true;
}

void BatteryComponent::updateInternal()
{
    if (millis() > lastBatteryCheck + BATTERY_CHECK_INTERVAL)
    {

        if (batteryPin.intValue() > 0)
        {

            values[valuesIndex] = analogRead(batteryPin.intValue());
            valuesIndex = (valuesIndex + 1) % VALUES_SIZE;
        }

        lastBatteryCheck = millis();
    }

    if (millis() > lastBatterySet + BATTERY_SET_INTERVAL)
    {
        if (chargePin.intValue() > 0)
        {
            isCharging.set(digitalRead(chargePin.intValue())); // measuredVal == 0;
        }

        if (batteryPin.intValue() > 0)
        {

            float sum = 0;
            for (int i = 0; i < VALUES_SIZE; i++)
                sum += values[i];

            float val = sum / VALUES_SIZE;

            const float maxVoltage = 4.2f;
            const float minVoltage = 3.5f;

            float relVal = (val - rawMin.intValue()) / (rawMax.intValue() - rawMin.intValue());
            float voltage = minVoltage + relVal * (maxVoltage - minVoltage);

            batteryLevel.set(voltage);
        }

        lastBatterySet = millis();
    }
}

void BatteryComponent::clearInternal()
{
}