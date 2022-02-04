#pragma once

#define MAX_CHILD_COMPONENTS 32
#define MAX_CHILD_CONTROLLABLES 32
#define MAX_EVENT_TYPES 16

class Component : public EventBroadcaster<ComponentEvent>
{
public:
    Component(const String &name) : name(name),
                                    isInit(false),
                                    numComponents(0),
                                    numParameters(0)
    {
        enabled = addParameter("enabled", true);
    }

    virtual ~Component() {}

    String name;
    bool isInit;
    Parameter *enabled;

    Component *components[MAX_CHILD_COMPONENTS];
    uint8_t numComponents;

    Parameter *parameters[MAX_CHILD_COMPONENTS];
    uint8_t numParameters;

    virtual String getComponentEventName(uint8_t type) const { return "[noname]"; }

    virtual void onChildComponentEvent(const ComponentEvent &e) {}

    bool init();
    void update();
    void clear();

    virtual bool initInternal() { return true; }
    virtual void updateInternal() {}
    virtual void clearInternal() {}

    void sendEvent(uint8_t type, var *data = NULL, int numData = 0)
    {
        EventBroadcaster::sendEvent(ComponentEvent(this, type, data, numData));
    }

    template <class T>
    T *addComponent() { return addComponent(new T()); };

    template <class T>
    T *addComponent(T *c)
    {
        components[numComponents] = (Component *)c;
        AddDefaultComponentListener(c);
        numComponents++;
        c->init();
        return c;
    }

    Component * getComponentWithName(const String& name)
    {
        if(name == this->name) return this;

        for(int i=0;i<numComponents;i++)
        {
            if(components[i]->name == name) return components[i];
        }

        return NULL;
    }

    Parameter *addParameter(const String &name, var val);

    virtual void onParameterEvent(const ParameterEvent &e) {}
    
    bool handleCommand(const String &command, var * data, int numData);
    virtual bool handleCommandInternal(const String &command, var * data, int numData) { return false; }
    bool checkCommand(const String &command, const String &ref, int numData, int expectedData);
};