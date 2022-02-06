
Parameter::Parameter(const String &name, var val, var _minVal, var _maxVal) : name(name),
                                                                              val(val),
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
    if (val == v && !force)
        return;

    switch (val.type)
    {
    case 'b':
        val.value.b = v.boolValue(); // force bool here
        break;

    case 'i':
    {
        if (hasRange())
            val.value.i = min(max(v.intValue(), minVal.intValue()), maxVal.intValue());
        else
            val.value.i = v.intValue();
    }
    break;

    case 'f':
    {
        if (hasRange()) 
            val.value.f = min(max(v.floatValue(), minVal.floatValue()), maxVal.floatValue());
        else
            val.value.f = v.floatValue();
    }
    break;

    case 's':
        val.value.s = (char *)v.stringValue().c_str();
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

void Parameter::fillJSONData(JsonObject o)
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
