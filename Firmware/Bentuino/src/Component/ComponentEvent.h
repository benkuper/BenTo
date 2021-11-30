#pragma once

class Component;

class ComponentEvent
{
public:
    ComponentEvent(Component * c, uint8_t type = -1, var * data = NULL, int numData = 0) :
        component(c),
        type(type),
        data(data),
        numData(numData)
        {

        }
        
    ~ComponentEvent(){}

    Component * component;
    uint8_t type;
    var * data;
    int numData;

    String getName() const;
};