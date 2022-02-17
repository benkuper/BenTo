ImplementSingleton(BatteryComponent);



bool BatteryComponent::initInternal(JsonObject o)
{
    timeSinceLastBatteryRead = 0;

    pin = AddConfigParameter("pin", 0);
    chargePin = AddConfigParameter("chargePin", 0);
    minRaw = AddConfigParameter("minRaw", 290);
    maxRaw = AddConfigParameter("maxRaw", 425);
    readRate = AddRangeConfigParameter("readRate", 1.0f, 0.1f, 10.f);

    rawValue = AddParameter("rawValue", 0);
    rawValue->readOnly = true;
    value = AddRangeParameter("value", 1.0f, 0.0f, 1.0f);
    value->readOnly = true;
    voltage = AddRangeParameter("voltage", 4.2f, 0, 4.2f);
    voltage->readOnly = true;
    lowBattery = AddParameter("lowBattery", false);
    lowBattery->readOnly = true;
    isCharging = AddParameter("isCharging", false);
    isCharging->readOnly = true;

    if (pin->intValue() == 0)
    {
        NDBG("Pin not set");
        return false;
    }

    analogSetPinAttenuation(pin->intValue(), ADC_0db);
    analogRead(pin->intValue()); // Just to get the ADC setup

    if (chargePin->intValue() > 0)
    {
        NDBG("Charge pin set to " + chargePin->stringValue());
        pinMode(chargePin->intValue(), INPUT);
    }

    return true;
}

void BatteryComponent::updateInternal()
{
    if (!enabled->boolValue())
        return;

    if (pin->intValue() == 0)
        return;

    long t = millis();

    if (lowBattery->boolValue())
    {
        if (t > timeAtLowBattery + lowBatteryShutdownTime)
        {
            sendEvent(CriticalBattery);
            return;
        }
    }

    if (t > timeSinceLastBatteryRead + (1000 / readRate->floatValue()))
    {
        // int raw = adc1_get_raw(ADC1_GPIO35_CHANNEL);//(adc1_channel_t)pin->intValue());
        int raw = analogRead(pin->intValue());

        // const uint32_t defaultVRef = 1100;
        // const float maxVoltage = 4.2f;
        // const float minVoltage = 3.5f;

        // esp_adc_cal_characteristics_t chars;
        // esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_0db, ADC_WIDTH_BIT_12, defaultVRef, &chars);
        // uint32_t mv = esp_adc_cal_raw_to_voltage(raw, &chars);
        // float v = (float)mv / 1000.0;

        rawValue->set(raw);

        voltage->set(MapF(raw, minRaw->floatValue(), maxRaw->floatValue(), 3.5f, 4.2f));
        value->set(Map01(voltage->floatValue(), 3.5f, 4.2f));

        // NDBG("Read, raw " + String(raw) + ", " + voltage->stringValue() + ", " + value->stringValue());

        lowBattery->set(value->floatValue() == 0);
        timeSinceLastBatteryRead = t;
    }
}

void BatteryComponent::clearInternal()
{
}

void BatteryComponent::onParameterEventInternal(const ParameterEvent &e)
{
    if (e.parameter == lowBattery)
    {
        if (lowBattery->boolValue())
            timeAtLowBattery = millis();
    }
}
