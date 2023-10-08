
Parameter::Parameter(const String &name, var val, var _minVal, var _maxVal, bool isConfig) : name(name),
                                                                                             val(val),
                                                                                             isConfig(isConfig),
                                                                                             readOnly(false),
                                                                                             options(nullptr),
                                                                                             optionsValues(nullptr),
                                                                                             numOptions(0)
{
    if (val.type == 'b')
        setRange(false, true);

    if (!_minVal.isVoid() && !_maxVal.isVoid())
        setRange(_minVal, _maxVal);
}

Parameter::~Parameter() {}

bool Parameter::boolValue() const { return val.boolValue(); }
int Parameter::intValue() const { return val.intValue(); }
float Parameter::floatValue() const { return val.floatValue(); }
String Parameter::stringValue() const { return val.stringValue(); }

bool Parameter::hasRange()
{
    return !minVal.isVoid() && !maxVal.isVoid();
}

void Parameter::set(const var &v, bool force, bool skipEnumEval)
{
    if (!skipEnumEval && numOptions > 0)
    {

        const var ev = getValForEnumValue(v);
        if (ev.isVoid())
        {
            DBG("Invalid enum value " + v.stringValue());
            return;
        }
        set(ev, force, true);
        return;
    }

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

var Parameter::getValForEnumValue(const var &v) const
{
    const String s = v.stringValue();
    for (int i = 0; i < numOptions; i++)
    {
        if (s == options[i])
        {
            if (optionsValues != nullptr)
                return optionsValues[i];
            return var(i);
        }
    }

    return var();
}

var Parameter::getEnumValueForVal(const var &v) const
{
    if (numOptions == 0)
        return var();

    if (optionsValues != nullptr)
    {
        for (int i = 0; i < numOptions; i++)
        {
            if (optionsValues[i] == v)
                return var(options[i]);
        }
    }
    else
    {
        int i = v.intValue();
        if (i >= 0 && i < numOptions)
            return var(options[i]);
    }

    return var();
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

void Parameter::fillSettingsData(JsonObject o, bool configOnly)
{
    // Only store value for now, should not require more

    const char *prop = configOnly ? name.c_str() : "value";

    switch (val.type)
    {
    case 'b':
        o[prop] = val.boolValue();
        break;
    case 'i':
        o[prop] = val.intValue();
        break;
    case 'f':
        o[prop] = val.floatValue();
        break;
    case 's':
        o[prop] = val.stringValue();
        break;
    }

    if (!configOnly)
    {
        o["type"] = val.type;
        o["readOnly"] = readOnly;
        o["isConfig"] = isConfig;
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

    if (options != nullptr && numOptions > 0)
    {
        JsonArray rArr = o.createNestedArray("RANGE");
        JsonObject vals = rArr.createNestedObject();
        JsonArray opt = vals.createNestedArray("VALS");

        for (int i = 0; i < numOptions; i++)
        {
            opt.add(options[i]);
        }

        o["TYPE"] = "s"; // force string type
        const var v = getEnumValueForVal(val);
        if (!v.isVoid())
            vArr.add(v.stringValue());
    }
    else
    {
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
}

var Parameter::getOSCQueryFeedbackData() const
{
    if (numOptions > 0)
    {
        const var v = getEnumValueForVal(val);
        if (!v.isVoid())
            return v;
    }

    return val;
}