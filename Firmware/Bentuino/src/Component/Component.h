#pragma once

#define MAX_CHILD_COMPONENTS 32
#define MAX_CHILD_PARAMETERS 32
// #define MAX_EVENT_TYPES 16

class Component : public EventBroadcaster<ComponentEvent>
{
public:
    Component(const String &name, bool _enabled = true) : name(name),
                                                          isInit(false),
                                                          parentComponent(NULL),
                                                          numComponents(0),
                                                          numParameters(0)
    {
    }

    virtual ~Component() {}
    virtual String getTypeString() const { return "[notype]"; }

    String name;

    bool isInit;
    DeclareConfigParameter(enabled, true);

    Component *parentComponent;

    Component *components[MAX_CHILD_COMPONENTS];
    uint8_t numComponents;

    Parameter *parameters[MAX_CHILD_PARAMETERS];
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
    T *addComponent(const String &name, bool _enabled, JsonObject o = JsonObject()) { return addComponent(new T(name, _enabled), o); };

    Component *addComponent(Component *c, JsonObject o = JsonObject());

    Component *getComponentWithName(const String &name);

    void addParameter(Parameter *param);
    // Parameter *addParameter(const String &name, var val, var minVal = var(), var maxVal = var(), bool isConfig = false);
    // Parameter *addConfigParameter(const String &name, var val, var minVal = var(), var maxVal = var()); // helpers for non ranged config param declaration simplification
    Parameter *getParameterWithName(const String &name);

    virtual void onParameterEvent(const ParameterEvent &e);
    virtual void onEnabledChanged() {}
    virtual void onParameterEventInternal(const ParameterEvent &e) {}

    bool handleCommand(const String &command, var *data, int numData);
    virtual bool handleCommandInternal(const String &command, var *data, int numData) { return false; }
    bool checkCommand(const String &command, const String &ref, int numData, int expectedData);

    virtual void fillSettingsData(JsonObject o, bool configOnly = false);
    virtual void fillOSCQueryData(JsonObject o, bool includeConfig = true, bool recursive = true);

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