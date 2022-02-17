#pragma once

#define MAX_CHILD_COMPONENTS 16
#define MAX_CHILD_CONTROLLABLES 16

enum ComponentType
{
    Type_Root,
    Type_Battery,
    Type_Button,
    Type_IO,
    Type_OSC,
    Type_Serial,
    Type_Comm,
    Type_Strip,
    Type_Layer,
    Type_BakeLayer,
    Type_StreamLayer,
    Type_StreamLayerReceiver,
    Type_ScriptLayer,
    Type_SystemLayer,
    Type_Script,
    Type_Servo,
    Type_Stepper,
    Type_Server,
    Type_Wifi,
    Type_Sequence,
    Type_IMU,
    Type_Files,
    TYPES_MAX
};

const String componentTypeNames[TYPES_MAX]{
    "Root",
    "Battery",
    "Button",
    "IO",
    "OSC",
    "Serial",
    "Comm",
    "Strip",
    "Layer",
    "BakeLayer",
    "StreamLayer",
    "StreamLayerReceiver",
    "ScriptLayer",
    "SystemLayer",
    "Script",
    "Servo",
    "Stepper",
    "Server",
    "Wifi",
    "Sequence",
    "IMU",
    "Files"};

class Component : public EventBroadcaster<ComponentEvent>
{
public:
    Component(ComponentType t, const String &_name = "") : type(t),
                                                  name(_name),
                                                  isInit(false),
                                                  parentComponent(NULL),
                                                  numComponents(0),
                                                  numOwnedComponents(0),
                                                  numParameters(0)
    {
    }

    virtual ~Component() {}
    virtual String getTypeString() const { return componentTypeNames[type]; }

    ComponentType type;
    String name;

    bool isInit;
    Parameter *enabled;
    Parameter *sendFeedback;

    Component *parentComponent;

    Component *components[MAX_CHILD_COMPONENTS];      // all components
    Component *ownedComponents[MAX_CHILD_COMPONENTS]; // components that are dynamically created
    uint8_t numComponents;
    uint8_t numOwnedComponents;

    Parameter *parameters[MAX_CHILD_CONTROLLABLES];
    uint8_t numParameters;

    virtual String getComponentEventName(uint8_t type) const { return "[noname]"; }

    virtual void onChildComponentEvent(const ComponentEvent &e) {}

    bool init(JsonObject o = JsonObject());
    void update();
    void clear();

    virtual bool initInternal(JsonObject o) { return true; }
    virtual void updateInternal() {}
    virtual void clearInternal() {}

    void sendEvent(uint8_t type, var *data = NULL, int numData = 0)
    {
        EventBroadcaster::sendEvent(ComponentEvent(this, type, data, numData));
    }

    template <class T>
    T *addComponent(const String &name, JsonObject o = JsonObject())
    {
        T *c = addComponent(new T(name), o);
        ownedComponents[numOwnedComponents] = c;
        numOwnedComponents++;
        return c;
    };

    void addComponent(Component *c, JsonObject o = JsonObject())
    {
        components[numComponents] = c;
        c->parentComponent = this;
        AddDefaultComponentListener(c);
        numComponents++;
        c->init(o);
    }

    Component *getComponentWithName(const String &name);

    Parameter *addParameter(const String &name, var val, var minVal = var(), var maxVal = var(), bool isConfig = false);
    Parameter *addConfigParameter(const String &name, var val, var minVal = var(), var maxVal = var()); // helpers for non ranged config param declaration simplification
    Parameter *getParameterWithName(const String &name);

    virtual void onParameterEvent(const ParameterEvent &e);
    virtual void onEnabledChanged() {}
    virtual void onParameterEventInternal(const ParameterEvent &e) {}

    bool handleCommand(const String &command, var *data, int numData);
    virtual bool handleCommandInternal(const String &command, var *data, int numData) { return false; }
    bool checkCommand(const String &command, const String &ref, int numData, int expectedData);

    virtual void fillSettingsData(JsonObject o, bool configOnly = false);
    virtual void fillOSCQueryData(JsonObject o, bool includeConfig = true);

    String getFullPath(bool includeRoot = false, bool scriptMode = false);

    // void scripting
    virtual void linkScriptFunctions(IM3Module module, bool isLocal = false);
    virtual void linkScriptFunctionsInternal(IM3Module module, const char *tName) {}

    // DeclareScriptFunctionVoid1(Component, setEnabled, uint32_t);
};