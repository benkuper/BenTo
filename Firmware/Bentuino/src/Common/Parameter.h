
#pragma once

class Parameter;

class ParameterEvent
{
public:
    ParameterEvent(Parameter * p, uint8_t type = -1, var * data = NULL, int numData = 0) :
        parameter(p),
        type(type),
        data(data),
        numData(numData)
        {

        }
        
    ~ParameterEvent(){}

    Parameter * parameter;
    uint8_t type;
    var * data;
    int numData;

    String getName() const;
};

class Parameter : public EventBroadcaster<ParameterEvent>
{
public:
    Parameter(const String &name, var val) : name(name), val(val) {}
    virtual ~Parameter() {}

    String name;
    var val;

    void set(const var &v, bool force = false) 
    {
        if(val == v && !force) return;
        val = v;
        sendEvent(ParameterEvent(this, ValueChanged, &val, 1));
    }

    DeclareEventTypes(ValueChanged);
};