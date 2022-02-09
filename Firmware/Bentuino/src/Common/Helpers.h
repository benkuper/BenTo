#define DBG(text) SerialComponent::instance->send(text)
#define NDBG(text) SerialComponent::instance->send("[" + name + "] " + text)

#define DeviceID RootComponent::instance->deviceID

// Class Helpers
#define DeclareSingleton(Class) static Class *instance;
#define ImplementSingleton(Class) Class *Class::instance = NULL;
#define InitSingleton() instance = this;
#define DeleteSingleton() instance = NULL;

// Event Helpers
#define DeclareEventTypes(...) enum EventTypes \
{                                              \
    __VA_ARGS__,                               \
    TYPES_MAX                                  \
};

// Component Helpers
#define AddComponent(name, comp, Type, enabled) comp = addComponent<Type##Component>(name, enabled, o["components"][name]);
#define AddDefaultComponentListener(comp) comp->addListener(std::bind(&Component::onChildComponentEvent, this, std::placeholders::_1));

// > Component Class definition
#define PDerive(Class) , public Class
#define DeclareComponentClass(ParentClass, ClassPrefix, ...) \
    class ClassPrefix##Component : public ParentClass        \
                                       __VA_ARGS__           \
    {                                                        \
    public:

#define DeclareSubComponent(ParentClass, ClassPrefix, Type, Derives)                                    \
    DeclareComponentClass(ParentClass, ClassPrefix, Derives)                                            \
        ClassPrefix##Component(const String &name, bool enabled = true) : ParentClass(name, enabled) {} \
    ~ClassPrefix##Component() {}                                                                        \
    virtual String getTypeString() const override { return Type; }

#define DeclareComponentSingleton(ClassPrefix, Type, Derives)                                                              \
    DeclareComponentClass(Component, ClassPrefix, Derives)                                                                 \
        DeclareSingleton(ClassPrefix##Component)                                                                           \
            ClassPrefix##Component(const String &name, bool enabled = true) : Component(name, enabled) { InitSingleton() } \
    ~ClassPrefix##Component() { DeleteSingleton() }                                                                        \
    virtual String getTypeString() const override { return Type; }

#define DeclareComponent(ClassPrefix, Type, Derives) DeclareSubComponent(Component, ClassPrefix, Type, Derives)

#define EndDeclareComponent \
    }                       \
    ;

// > Events
#define DeclareComponentEventTypes(...) enum ComponentEventTypes \
{                                                                \
    __VA_ARGS__,                                                 \
    TYPES_MAX                                                    \
};
#define DeclareComponentEventNames(...)                       \
    const String componentEventNames[TYPES_MAX]{__VA_ARGS__}; \
    String getComponentEventName(uint8_t type) const override { return componentEventNames[type]; }

// Command Helpers
#define CheckCommand(cmd, num) checkCommand(command, cmd, numData, num)
#define CommandCheck(cmd, Count)                                                          \
    if (command == cmd)                                                                   \
    {                                                                                     \
        if (numData < Count)                                                              \
        {                                                                                 \
            NDBG("setConfig needs 2 parameters, only " + String(numData) + " provided."); \
            return false;                                                                 \
        }                                                                                 \
        else                                                                              \
        {
#define ElifCommandCheck(cmd, Count) EndCommandCheck else CommandCheck(cmd, Count)
#define EndCommandCheck \
    }                   \
    }

// Parameter Helpers
#define AddDefaultParameterListener(Class, param) param->addListener(std::bind(&Class::onParameterEvent, this, std::placeholders::_1));
#define SendParameterFeedback(param) CommunicationComponent::instance->sendParameterFeedback(this, param);

#define AddParameter(name, val) addParameter(name, val)
#define AddRangeParameter(name, val, minVal, maxVal, isConfig) addParameter(name, val, minVal, maxVal, false)
//Only config parameters check the settings
#define AddConfigParameter(name, val) addParameter(name, Settings::getVal(o, name, val), var(), var(), true)
#define AddRangeConfigParameter(name, val, minVal, maxVal) addParameter(name, Settings::getVal(o, name, val), minVal, maxVal, true)

// Config (obsolete)

//#define SendConfigFeedback(configName) CommunicationComponent::instance->sendConfigFeedback(this, configName, GetStringConfig(configName));

// Preferences
// #define GetConfig(sname, Class) SettingsComponent::instance->settings[name][sname].as<Class>()
// #define GetStringConfig(sname) GetConfig(sname, String)
// #define GetIntConfig(sname) GetConfig(sname, int)
// #define GetFloatConfig(sname) GetConfig(sname, float)
// #define GetBoolConfig(sname) GetConfig(sname, bool)

// #define SetConfig(sname,val) SettingsComponent::instance->setConfig(name, sname, val, true);
// #define SetConfigSave(sname,val) SettingsComponent::instance->setConfig(name, sname, val, false);
