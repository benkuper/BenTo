#pragma once

#define MAX_CHILD_COMPONENTS 32
#define MAX_CHILD_CONTROLLABLES 32
#define MAX_EVENT_TYPES 16

class Component : public EventBroadcaster<ComponentEvent>
{
public:
    Component(const String &name) : name(name),
                                    numComponents(0)
    {
        enabled = addParameter<bool>("enabled", true);
    }

    virtual ~Component() {}

    String name;
    Parameter<bool> *enabled;

    Component *components[MAX_CHILD_COMPONENTS];
    uint8_t numComponents;

    Controllable *controllables[MAX_CHILD_COMPONENTS];
    uint8_t numControllables;

    virtual String getEventName(uint8_t type) const { return "[noname]"; }

    virtual void onChildComponentEvent(const ComponentEvent &e) {}

    void init()
    {
        initInternal();
    }

    void update()
    {
        for (int i = 0; i < numComponents; i++)
            components[i]->update();
            
        updateInternal();
    }

    void clear()
    {
        clearInternal();
        for (int i = 0; i < numComponents; i++)
        {
            components[i]->clear();
            delete components[i];
        }
        numComponents = 0;
    }

    virtual void initInternal() {}
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

    template <class T>
    Parameter<T> *addParameter(const String &name, T val)
    {
        Parameter<T> *p = new Parameter<T>(name, val);
        controllables[numControllables] = p;
        // AddDefaultParameterListener(p)
        return p;
    }
};