
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

    const String * options;
    const var* optionsValues;
    int numOptions;

    bool boolValue() const;
    int intValue() const;
    float floatValue() const;
    String stringValue() const;

    var getValForEnumValue(const var& v) const;
    var getEnumValueForVal(const var& v) const;
    bool hasRange();

    void set(const var &v, bool force = false, bool skipEnumEval = false);

    void setRange(var newMin, var newMax);

    void fillSettingsData(JsonObject o, bool configOnly);
    void fillOSCQueryData(JsonObject o);
    var getOSCQueryFeedbackData() const;

    DeclareEventTypes(ValueChanged);
};