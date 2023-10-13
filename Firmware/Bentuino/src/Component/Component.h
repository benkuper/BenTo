#pragma once

#define MAX_CHILD_COMPONENTS 16
#define MAX_CHILD_PARAMS 20

class Component : public EventBroadcaster<ComponentEvent>
{
public:
    Component(const String &name, bool _enabled = true) : name(name),
                                                          isInit(false),
                                                          parentComponent(NULL),
                                                          numComponents(0),
                                                          numParams(0)
    //   numParameters(0)
    {
    }

    virtual ~Component() {}
    virtual String getTypeString() const { return "[notype]"; }

    String name;
    bool isInit;

    DeclareBoolParam(enabled, true);

    Component *parentComponent;

    Component *components[MAX_CHILD_COMPONENTS];
    uint8_t numComponents;

    // Parameter *parameters[MAX_CHILD_PARAMETERS];
    // uint8_t numParameters;

    enum ParamType
    {
        Trigger,
        Bool,
        Int,
        Float,
        Str,
        P2D,
        P3D,
        ParamTypeMax
    };
    const String typeNames[ParamTypeMax]{"I", "b", "i", "f", "s", "ff", "fff"};

    void *params[MAX_CHILD_PARAMS];
    ParamType paramTypes[MAX_CHILD_PARAMS];

    uint8_t numParams;

    virtual String getComponentEventName(uint8_t type) const
    {
        return "[noname]";
    }

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
    T *addComponent(const String &name, bool _enabled, JsonObject o = JsonObject()) { return (T *)addComponent(new T(name, _enabled), o); };

    Component *addComponent(Component *c, JsonObject o = JsonObject());

    Component *getComponentWithName(const String &name);

    void addParam(void *param, ParamType type);
    void setParam(void *param, var *value, int nmData);
    ParamType getParamType(void *param) const;
    String getParamString(void *param) const;

    virtual void onEnabledChanged() {}

    void paramValueChanged(void *param);
    virtual void paramValueChangedInternal(void *param) {}
    virtual void childParamValueChanged(Component *caller, Component *comp, void *param);
    virtual bool checkParamsFeedback(void *param);
    virtual bool checkParamsFeedbackInternal(void *param) { return false; }
    // virtual void sendParamFeedback(void* param);

    bool handleCommand(const String &command, var *data, int numData);
    virtual bool handleCommandInternal(const String &command, var *data, int numData) { return false; }
    bool checkCommand(const String &command, const String &ref, int numData, int expectedData);

    bool handleSetParam(const String &paramName, var *data, int numData);
    virtual bool handleSetParamInternal(const String &paramName, var *data, int numData) { return false; }

    void fillSettingsData(JsonObject o, bool configOnly = false);
    virtual void fillSettingsParamsInternal(JsonObject o, bool configOnly = false) {}

    virtual void fillOSCQueryData(JsonObject o, bool includeConfig = true, bool recursive = true);
    virtual void fillOSCQueryParamsInternal(JsonObject o, const String &fullPath) {}
    virtual void fillOSCQueryParam(JsonObject o, const String &fullPath, const String &pName, ParamType t, void *param, bool readOnly = false, const String *options = nullptr, int numOptions = 0, float vMin = 0, float vMax = 0);

    enum OSCQueryChunkType
    {
        Start,
        Content,
        End,
        ChunkTypeMax
    };

    struct OSCQueryChunk
    {
        OSCQueryChunk(Component *c, OSCQueryChunkType t = Start, String d = "") : nextComponent(c), nextType(t), data(d) {}
        Component *nextComponent = nullptr;
        OSCQueryChunkType nextType = Start;
        String data = "";
    };

    void fillChunkedOSCQueryData(OSCQueryChunk *chunk);
    void setupChunkAfterComponent(OSCQueryChunk *result, const Component *c);

    String getFullPath(bool includeRoot = false, bool scriptMode = false) const;

// void scripting
#ifdef USE_SCRIPT
    virtual void linkScriptFunctions(IM3Module module, bool isLocal = false);
    virtual void linkScriptFunctionsInternal(IM3Module module, const char *tName) {}
#endif
    // DeclareScriptFunctionVoid1(Component, setEnabled, uint32_t);
};