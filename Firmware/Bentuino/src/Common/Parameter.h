
#pragma once

class Parameter;

class ParameterEvent
{
public:
    ParameterEvent(Parameter *p, uint8_t type = -1, var *data = NULL, int numData = 0) : parameter(p),
                                                                                         type(type),
                                                                                         data(data),
                                                                                         numData(numData)
    {
    }

    ~ParameterEvent() {}

    Parameter *parameter;
    uint8_t type;
    var *data;
    int numData;

    String getName() const;
};

class Parameter : public EventBroadcaster<ParameterEvent>
{
public:
    Parameter(const String &name, var val, var minVal = var(), var maxVal = var(), bool isConfig = false);

    virtual ~Parameter();

    String name;
    var val;
    var minVal;
    var maxVal;
    bool isConfig;
    bool readOnly;

    bool boolValue();
    int intValue();
    float floatValue();
    String stringValue();

    bool hasRange();

    void set(const var &v, bool force = false);

    void setRange(var newMin, var newMax);

    void fillSettingsData(JsonObject o, bool configOnly);
    void fillOSCQueryData(JsonObject o);

    DeclareEventTypes(ValueChanged);
};