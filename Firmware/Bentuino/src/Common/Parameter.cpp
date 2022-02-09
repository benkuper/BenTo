
Parameter::Parameter(const String &name, var val, var _minVal, var _maxVal, bool isConfig) : name(name),
                                                                                             val(val),
                                                                                             isConfig(isConfig),
                                                                                             readOnly(false)
{
    if (val.type == 'b')
        setRange(false, true);

    if (!_minVal.isVoid() && !_maxVal.isVoid())
        setRange(_minVal, _maxVal);

}

Parameter::~Parameter() {}

bool Parameter::boolValue() { return val.boolValue(); }
int Parameter::intValue() { return val.intValue(); }
float Parameter::floatValue() { return val.floatValue(); }
String Parameter::stringValue() { return val.stringValue(); }

bool Parameter::hasRange()
{
    return !minVal.isVoid() && !maxVal.isVoid();
}

void Parameter::set(const var &v, bool force)
{
    switch (val.type)
    {
    case 'b':
        if (val.boolValue() == v.boolValue() && !force)
            return;

        val = v.boolValue();
        break;

    case 'i':
    {
        int newVal = val.intValue();
        if (hasRange())
            newVal = min(max(v.intValue(), minVal.intValue()), maxVal.intValue());
        else
            newVal = v.intValue();

        if (val.intValue() == v.intValue() && !force)
            return;
        val = newVal;
    }
    break;

    case 'f':
    {
        float newVal = val.intValue();

        if (hasRange())
            newVal = min(max(v.floatValue(), minVal.floatValue()), maxVal.floatValue());
        else
            newVal = v.floatValue();

        if (val.floatValue() == v.floatValue() && !force)
            return;

        val = newVal;
    }
    break;

    case 's':
        if (val.stringValue() == v.stringValue() && !force)
            return;

        val = v.s;
        break;

    default:
        break;
    }

    sendEvent(ParameterEvent(this, ValueChanged, &val, 1));
}

void Parameter::setRange(var newMin, var newMax)
{
    switch (val.type)
    {
    case 'f':
        minVal = newMin.floatValue();
        maxVal = newMax.floatValue();
        break;

    case 'i':
        minVal = newMin.intValue();
        maxVal = newMax.intValue();
        break;

    case 'b':
        minVal = newMin.boolValue();
        maxVal = newMax.boolValue();
        break;

    default:
        NDBG("Type " + String(val.type) + " does not support range");
        break;
    }
}

void Parameter::fillSettingsData(JsonObject o)
{
    o["type"] = val.type;
    o["readOnly"] = readOnly;
    o["isConfig"] = isConfig;

    // Only store value for now, should not require more
    switch (val.type)
    {
    case 'b':
        o["value"] = val.boolValue();
        break;
    case 'i':
        o["value"] = val.intValue();
        break;
    case 'f':
        o["value"] = val.floatValue();
        break;
    case 's':
        o["value"] = val.stringValue();
        break;
    }
}

void Parameter::fillOSCQueryData(JsonObject o)
{
    o["DESCRIPTION"] = name;
    o["ACCESS"] = readOnly ? 1 : 3;
    String type(val.type);
    if (val.type == 'b')
        type = val.boolValue() ? "T" : "F";
    o["TYPE"] = type;

    JsonArray vArr = o.createNestedArray("VALUE");

    switch (val.type)
    {
    case 'b':
        vArr.add(val.boolValue());
        break;

    case 'i':
        vArr.add(val.intValue());
        break;

    case 'f':
        vArr.add(val.floatValue());
        break;

    case 's':
        vArr.add(val.stringValue());
        break;
    }

    if (hasRange())
    {
        JsonArray rArr = o.createNestedArray("RANGE");
        JsonObject ro = rArr.createNestedObject();

        switch (val.type)
        {
        case 'b':
        case 'i':
            ro["MIN"] = minVal.intValue();
            ro["MAX"] = maxVal.intValue();
            break;

        case 'f':
            ro["MIN"] = minVal.floatValue();
            ro["MAX"] = maxVal.floatValue();
            break;
        }
    }
    else
    {

        o["RANGE"] = nullptr;
    }
}
