#define STR(x) #x
#define XSTR(x) STR(x)
#define COMMA ,

// #define DBG(t)
// #define NDBG(t)
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
// Only config parameters check the settings
#define AddConfigParameter(name, val) addParameter(name, Settings::getVal(o, name, val), var(), var(), true)
#define AddRangeConfigParameter(name, val, minVal, maxVal) addParameter(name, Settings::getVal(o, name, val), minVal, maxVal, true)

// Script

#define LinkScriptFunctionsStart                                                                     \
    virtual void linkScriptFunctionsInternal(IM3Module module, const char *tName) override \
    {
#define LinkScriptFunctionsEnd }

#define LinkScriptFunction(Class, FunctionName, ReturnType, Args) m3_LinkRawFunctionEx(module, tName, XSTR(FunctionName), XSTR(ReturnType(Args)), &Class::m3_##FunctionName, this);

#define DeclareScriptFunctionVoid(Class, FunctionName, CallArgs, DeclarationArgs, GetArgs) \
    static m3ApiRawFunction(m3_##FunctionName)                                             \
    {                                                                                      \
        GetArgs;                                                                           \
        static_cast<Class *>(_ctx->userdata)->FunctionName##FromScript(CallArgs);          \
        m3ApiSuccess();                                                                    \
    }                                                                                      \
    virtual void FunctionName##FromScript(DeclarationArgs)

#define DeclareScriptFunctionReturn(Class, FunctionName, ReturnType, CallArgs, DeclarationArgs, GetArgs) \
    static m3ApiRawFunction(m3_##FunctionName)                                                           \
    {                                                                                                    \
        m3ApiReturnType(ReturnType);                                                                     \
        GetArgs;                                                                                         \
        ReturnType result = static_cast<Class *>(_ctx->userdata)->FunctionName##FromScript(CallArgs);    \
        m3ApiReturn(result);                                                                             \
    }                                                                                                    \
    virtual ReturnType FunctionName##FromScript(DeclarationArgs)

#define SA(Type, index) m3ApiGetArg(Type, arg##index);
#define CA(Type, index) Type arg##index

#define DeclareScriptFunctionVoid0(Class, FunctionName) DeclareScriptFunctionVoid(Class, FunctionName, , , )
#define DeclareScriptFunctionVoid1(Class, FunctionName, Type1) DeclareScriptFunctionVoid(Class, FunctionName, arg1, CA(Type1, 1), SA(Type1, 1))
#define DeclareScriptFunctionVoid2(Class, FunctionName, Type1, Type2) DeclareScriptFunctionVoid(Class, FunctionName, arg1 COMMA arg2, CA(Type1, 1) COMMA CA(Type2, 2), SA(Type1, 1) SA(Type2, 2))
#define DeclareScriptFunctionVoid3(Class, FunctionName, Type1, Type2, Type3) DeclareScriptFunctionVoid(Class, FunctionName, arg1 COMMA arg2 COMMA arg3, CA(Type1, 1) COMMA CA(Type2, 2) COMMA CA(Type3, 3), SA(Type1, 1) SA(Type2, 2) SA(Type3, 3))

#define DeclareScriptFunctionReturn0(Class, FunctionName, ReturnType) DeclareScriptFunctionReturn(Class, FunctionName, ReturnType, , , )
#define DeclareScriptFunctionReturn1(Class, FunctionName, ReturnType, Type1) DeclareScriptFunctionReturn(Class, FunctionName, ReturnType, arg1, CA(Type1, 1), SA(Type1, 1))
#define DeclareScriptFunctionReturn2(Class, FunctionName, ReturnType, Type1, Type2) DeclareScriptFunctionReturn(Class, FunctionName, ReturnType, arg1 COMMA arg2, CA(Type1, 1) COMMA CA(Type2, 2), SA(Type1, 1) SA(Type2, 2))
#define DeclareScriptFunctionReturn3(Class, FunctionName, ReturnType, Type1, Type2, Type3) DeclareScriptFunctionReturn(Class, FunctionName, ReturnType, arg1 COMMA arg2 COMMA arg3, CA(Type1, 1) COMMA CA(Type2, 2) COMMA CA(Type3, 3), SA(Type1, 1) SA(Type2, 2) SA(Type3, 3))