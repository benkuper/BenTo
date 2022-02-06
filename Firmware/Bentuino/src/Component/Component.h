#pragma once

#define MAX_CHILD_COMPONENTS 32
#define MAX_CHILD_CONTROLLABLES 32
#define MAX_EVENT_TYPES 16

class Component : public EventBroadcaster<ComponentEvent>
{
public:
    Component(const String &name, bool _enabled = true) : name(name),
                                    isInit(false),
                                    parentComponent(NULL),
                                    numComponents(0),
                                    numParameters(0)
    {
        enabled = addParameter("enabled", _enabled);
    }

    virtual ~Component() {}

    String name;
    bool isInit;
    Parameter *enabled;

    Component *parentComponent;

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
    T *addComponent(bool _enabled) { return addComponent(new T(_enabled)); };

    template <class T>
    T *addComponent(T *c)
    {
        components[numComponents] = (Component *)c;
        c->parentComponent = this;
        AddDefaultComponentListener(c);
        numComponents++;
        c->init();
        return c;
    }

    Component *getComponentWithName(const String &name)
    {
        if (name == this->name)
            return this;

        int subCompIndex = name.indexOf('.');

        if (subCompIndex > 0)
        {
            String n = name.substring(0, subCompIndex);
            for (int i = 0; i < numComponents; i++)
            {
                if (components[i]->name == n)
                    return components[i]->getComponentWithName(name.substring(subCompIndex + 1));
            }
        }
        else
        {
            for (int i = 0; i < numComponents; i++)
            {
                if (components[i]->name == name)
                    return components[i];
            }
        }

        return NULL;
    }

    Parameter* addParameter(const String &name, var val, var minVal = var(), var maxVal = var());

    virtual void onParameterEvent(const ParameterEvent &e);
    virtual void onEnabledChanged() {}
    virtual void onParameterEventInternal(const ParameterEvent &e) {}

    bool handleCommand(const String &command, var *data, int numData);
    virtual bool handleCommandInternal(const String &command, var *data, int numData) { return false; }
    bool checkCommand(const String &command, const String &ref, int numData, int expectedData);

    virtual void fillJSONData(JsonObject o);
    String getFullPath();
};