#define STR(x) #x
#define XSTR(x) STR(x)
#define COMMA ,

// #define DBG(t)
// #define NDBG(t)
#define DBG(text) SerialComponent::instance->send(text)
#define NDBG(text) SerialComponent::instance->send("[" + name + "] " + text)

#define DeviceID RootComponent::instance->deviceID
#define DeviceType RootComponent::instance->deviceType
#define DeviceName RootComponent::instance->deviceName

// Class Helpers
#define DeclareSingleton(Class) static Class *instance;
#define ImplementSingleton(Class) Class *Class::instance = NULL;
#define ImplementManagerSingleton(Class) Class##ManagerComponent *Class##ManagerComponent::instance = NULL;
#define InitSingleton() instance = this;
#define DeleteSingleton() instance = NULL;

// Event Helpers
#define DeclareEventTypes(...) enum EventTypes \
{                                              \
    __VA_ARGS__,                               \
    TYPES_MAX                                  \
};

// Component Helpers
#define AddComponent(comp, name, Type, enabled) comp = addComponent<Type##Component>(name, enabled, o["components"][name]);
#define AddOwnedComponent(comp) addComponent(comp, o["components"][(comp)->name]);
#define AddDefaultComponentListener(comp) comp->addListener(std::bind(&Component::onChildComponentEvent, this, std::placeholders::_1));

// > Component Class definition
#define PDerive(Class) , public Class
#define DeclareComponentClass(ParentClass, ClassPrefix, ...) \
    class ClassPrefix##Component : public ParentClass        \
                                       __VA_ARGS__           \
    {                                                        \
    public:

#define DeclareSubComponent(ParentClass, ClassPrefix, Type, Derives)                                           \
    DeclareComponentClass(ParentClass, ClassPrefix, Derives)                                                   \
        ClassPrefix##Component(const String &name = Type, bool enabled = true) : ParentClass(name, enabled) {} \
    ~ClassPrefix##Component() {}                                                                               \
    virtual String getTypeString() const override { return Type; }

#define DeclareComponentSingleton(ClassPrefix, Type, Derives)                                                                     \
    DeclareComponentClass(Component, ClassPrefix, Derives)                                                                        \
        DeclareSingleton(ClassPrefix##Component)                                                                                  \
            ClassPrefix##Component(const String &name = Type, bool enabled = true) : Component(name, enabled) { InitSingleton() } \
    ~ClassPrefix##Component() { DeleteSingleton() }                                                                               \
    virtual String getTypeString() const override { return Type; }

#define DeclareComponent(ClassPrefix, Type, Derives) DeclareSubComponent(Component, ClassPrefix, Type, Derives)

#define EndDeclareComponent \
    }                       \
    ;

// Manager

#ifdef MANAGER_USE_STATIC_ITEMS
#define DefineStaticItems(Type, MType) Type##Component items[MType##_MAX_COUNT];
#define AddStaticOrDynamicComponent(name, Type, enabled) \
    items[i].name = name;                                \
    items[i].enabled = enabled;                          \
    AddOwnedComponent(&items[i]);
#else
#define DefineStaticItems(Type, MType) Type##Component *items[MType##_MAX_COUNT];
#define AddStaticOrDynamicComponent(name, Type, enabled) AddComponent(items[i], name, Type, enabled);
#endif

#define DeclareComponentManager(Type, MType, mName, itemName) \
    DeclareComponentSingleton(Type##Manager, #mName, )        \
        DeclareIntParam(count, 1);                            \
                                                              \
    DefineStaticItems(Type, MType);                           \
    bool initInternal(JsonObject o) override                  \
    {                                                         \
        AddIntParam(count);                                   \
        for (int i = 0; i < count; i++)                       \
        {                                                     \
            String n = #itemName + String(i + 1);             \
            AddStaticOrDynamicComponent(n, Type, i == 0);     \
        }                                                     \
        return true;                                          \
    }                                                         \
    HandleSetParamInternalStart                               \
        CheckAndSetParam(count);                              \
    HandleSetParamInternalEnd;                                \
    FillSettingsInternalStart                                 \
        FillSettingsParam(count);                             \
    FillSettingsInternalEnd;                                  \
    FillOSCQueryInternalStart                                 \
        FillOSCQueryIntParam(count);                          \
    FillOSCQueryInternalEnd

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

// Parameters

// Class-less parameter system
#define DeclareBoolParam(name, val) bool name = val;
#define DeclareIntParam(name, val) int name = val;
#define DeclareFloatParam(name, val) float name = val;
#define DeclareStringParam(name, val) String name = val;
#define DeclareP2DParam(name, val1, val2) float name[2]{val1, val2};
#define DeclareP3DParam(name, val1, val2, val3) float name[3]{val1, val2, val3};

#define AddParamWithTag(type, class, param, tag) \
    addParam(&param, ParamType::type, tag);      \
    param = Settings::getVal<class>(o, #param, param);

#define AddBoolParamWithTag(param, tag) AddParamWithTag(Bool, bool, param, tag)
#define AddIntParamWithTag(param, tag) AddParamWithTag(Int, int, param, tag)
#define AddFloatParamWithTag(param, tag) AddParamWithTag(Float, float, param, tag)
#define AddStringParamWithTag(param, tag) AddParamWithTag(Str, String, param, tag)
#define AddP2DParamWithTag(param, tag) addParam(&param, ParamType::P2D, tag);
#define AddP3DParamWithTag(param, tag) addParam(&param, ParamType::P3D, tag);

#define AddBoolParam(param) AddBoolParamWithTag(param, TagNone)
#define AddIntParam(param) AddIntParamWithTag(param, TagNone)
#define AddFloatParam(param) AddFloatParamWithTag(param, TagNone)
#define AddStringParam(param) AddStringParamWithTag(param, TagNone)
#define AddP2DParam(param) AddP2DParamWithTag(param, TagNone)
#define AddP3DParam(param) AddP3DParamWithTag(param, TagNone)

#define AddBoolParamConfig(param) AddBoolParamWithTag(param, TagConfig)
#define AddIntParamConfig(param) AddIntParamWithTag(param, TagConfig)
#define AddFloatParamConfig(param) AddFloatParamWithTag(param, TagConfig)
#define AddStringParamConfig(param) AddStringParamWithTag(param, TagConfig)
#define AddP2DParamConfig(param) AddP2DParamWithTag(param, TagConfig)
#define AddP3DParamConfig(param) AddP3DParamWithTag(param, TagConfig)

#define SetParam(param, val)        \
    {                               \
        var pData[1];               \
        pData[0] = val;             \
        setParam(&param, pData, 1); \
    };
#define SetParam2(param, val1, val2) \
    {                                \
        var pData[2];                \
        pData[0] = val1;             \
        pData[1] = val2;             \
        setParam(&param, pData, 2);  \
    };
#define SetParam3(param, val, val2, val3) \
    {                                     \
        var pData[3];                     \
        pData[0] = val1;                  \
        pData[1] = val2;                  \
        pData[2] = val3;                  \
        setParam(&param, pData, 3);       \
    };

// Handle Check and Set

#define HandleSetParamInternalStart                                                               \
    virtual bool handleSetParamInternal(const String &paramName, var *data, int numData) override \
    {

#define HandleSetParamInternalMotherClass(Class)                 \
    if (Class::handleSetParamInternal(paramName, data, numData)) \
        return true;

#define CheckAndSetParam(param)                      \
    {                                                \
        if (paramName == #param)                     \
        {                                            \
            setParam((void *)&param, data, numData); \
            return true;                             \
        }                                            \
    }

#define CheckAndSetEnumParam(param, options, numOption) \
    {                                                   \
        if (paramName == #param)                        \
        {                                               \
            var newData[1];                             \
            if (data[0].type == 's')                    \
            {                                           \
                String s = data[0].stringValue();       \
                for (int i = 0; i < numOption; i++)     \
                {                                       \
                    if (s == options[i])                \
                    {                                   \
                        newData[0] = i;                 \
                        break;                          \
                    }                                   \
                };                                      \
            }                                           \
            else                                        \
                newData[0] = data[0].intValue();        \
                                                        \
            setParam((void *)&param, newData, 1);       \
            return true;                                \
        }                                               \
    }

#define HandleSetParamInternalEnd \
    return false;                 \
    }

// Feedback

#define CheckFeedbackParamInternalStart                   \
    virtual bool checkParamsFeedbackInternal(void *param) \
    {

#define CheckFeedbackParamInternalMotherClass(Class) \
    if (Class::checkParamsFeedbackInternal(param))   \
        return true;

#define CheckAndSendParamFeedback(p)                                 \
    {                                                                \
        if (param == (void *)&p)                                     \
        {                                                            \
            SendParamFeedback(this, param, #p, getParamType(param)); \
            return true;                                             \
        }                                                            \
    }

#define CheckFeedbackParamInternalEnd \
    return false;                     \
    }

#define SendParamFeedback(comp, param, pName, type) CommunicationComponent::instance->sendParamFeedback(this, param, pName, type);

// Fill Settings

#define FillSettingsParam(param) \
    {                            \
        o[#param] = param;       \
    }

#define FillSettingsInternalMotherClass(Class) Class::fillSettingsParamsInternal(o, showConfig);

#define FillSettingsInternalStart                                                           \
    virtual void fillSettingsParamsInternal(JsonObject o, bool showConfig = false) override \
    {
#define FillSettingsInternalEnd }

// Fill OSCQuery
#define FillOSCQueryBoolParam(param) fillOSCQueryParam(o, fullPath, #param, ParamType::Bool, &param, showConfig);
#define FillOSCQueryIntParam(param) fillOSCQueryParam(o, fullPath, #param, ParamType::Int, &param, showConfig);
#define FillOSCQueryEnumParam(param, options, numOptions) fillOSCQueryParam(o, fullPath, #param, ParamType::Int, &param, showConfig, false, options, numOptions);
#define FillOSCQueryFloatParam(param) fillOSCQueryParam(o, fullPath, #param, ParamType::Float, &param, showConfig);
#define FillOSCQueryRangeParam(param, vMin, vMax) fillOSCQueryParam(o, fullPath, #param, ParamType::Float, &param, showConfig, false, nullptr, 0, vMin, vMax);
#define FillOSCQueryStringParam(param) fillOSCQueryParam(o, fullPath, #param, ParamType::Str, &param, showConfig);

#define FillOSCQueryBoolParamReadOnly(param) fillOSCQueryParam(o, fullPath, #param, ParamType::Bool, &param, showConfig, true);
#define FillOSCQueryIntParamReadOnly(param) fillOSCQueryParam(o, fullPath, #param, ParamType::Int, &param, showConfig, true);
#define FillOSCQueryEnumParamReadOnly(param, options, numOptions) fillOSCQueryParam(o, fullPath, #param, ParamType::Int, &param, showConfig, true, options, numOptions);
#define FillOSCQueryFloatParamReadOnly(param) fillOSCQueryParam(o, fullPath, #param, ParamType::Float, &param, showConfig, true);
#define FillOSCQueryRangeParamReadOnly(param, vMin, vMax) fillOSCQueryParam(o, fullPath, #param, ParamType::Float, &param, showConfig, true, nullptr, 0, vMin, vMax);
#define FillOSCQueryStringParamReadOnly(param) fillOSCQueryParam(o, fullPath, #param, ParamType::Str, &param, showConfig, true);

#define FillOSCQueryInternalStart                                                 \
    virtual void fillOSCQueryParamsInternal(JsonObject o, const String &fullPath, bool showConfig = true) \
    {
#define FillOSCQueryInternalEnd }

#define FillOSCQueryInternalMotherClass(Class) Class::fillOSCQueryParamsInternal(o, fullPath, showConfig);

// Script

#define LinkScriptFunctionsStart                                                           \
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
#define DeclareScriptFunctionVoid4(Class, FunctionName, Type1, Type2, Type3, Type4) DeclareScriptFunctionVoid(Class, FunctionName, arg1 COMMA arg2 COMMA arg3 COMMA arg4, CA(Type1, 1) COMMA CA(Type2, 2) COMMA CA(Type3, 3) COMMA CA(Type4, 4), SA(Type1, 1) SA(Type2, 2) SA(Type3, 3) SA(Type4, 4))
#define DeclareScriptFunctionVoid5(Class, FunctionName, Type1, Type2, Type3, Type4, Type5) DeclareScriptFunctionVoid(Class, FunctionName, arg1 COMMA arg2 COMMA arg3 COMMA arg4 COMMA arg5, CA(Type1, 1) COMMA CA(Type2, 2) COMMA CA(Type3, 3) COMMA CA(Type4, 4) COMMA CA(Type5, 5), SA(Type1, 1) SA(Type2, 2) SA(Type3, 3) SA(Type5, 4) SA(Type5, 5))
#define DeclareScriptFunctionVoid6(Class, FunctionName, Type1, Type2, Type3, Type4, Type5, Type6) DeclareScriptFunctionVoid(Class, FunctionName, arg1 COMMA arg2 COMMA arg3 COMMA arg4 COMMA arg5 COMMA arg6, CA(Type1, 1) COMMA CA(Type2, 2) COMMA CA(Type3, 3) COMMA CA(Type4, 4) COMMA CA(Type5, 5) COMMA CA(Type6, 6), SA(Type1, 1) SA(Type2, 2) SA(Type3, 3) SA(Type4, 4) SA(Type5, 5) SA(Type6, 6))

#define DeclareScriptFunctionReturn0(Class, FunctionName, ReturnType) DeclareScriptFunctionReturn(Class, FunctionName, ReturnType, , , )
#define DeclareScriptFunctionReturn1(Class, FunctionName, ReturnType, Type1) DeclareScriptFunctionReturn(Class, FunctionName, ReturnType, arg1, CA(Type1, 1), SA(Type1, 1))
#define DeclareScriptFunctionReturn2(Class, FunctionName, ReturnType, Type1, Type2) DeclareScriptFunctionReturn(Class, FunctionName, ReturnType, arg1 COMMA arg2, CA(Type1, 1) COMMA CA(Type2, 2), SA(Type1, 1) SA(Type2, 2))
#define DeclareScriptFunctionReturn3(Class, FunctionName, ReturnType, Type1, Type2, Type3) DeclareScriptFunctionReturn(Class, FunctionName, ReturnType, arg1 COMMA arg2 COMMA arg3, CA(Type1, 1) COMMA CA(Type2, 2) COMMA CA(Type3, 3), SA(Type1, 1) SA(Type2, 2) SA(Type3, 3))