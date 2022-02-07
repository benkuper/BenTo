#define DBG(text) SerialComponent::instance->send(text)
#define NDBG(text) SerialComponent::instance->send("["+name+"] "+text)

#define DeclareSingleton(Class) static Class * instance;
#define ImplementSingleton(Class) Class * Class::instance = NULL;
#define InitSingleton() instance = this;
#define DeleteSingleton() instance = NULL;

//Event Helpers
#define DeclareEventTypes(...) enum EventTypes { __VA_ARGS__, TYPES_MAX };


//Parameter Helpers
#define AddDefaultParameterListener(Class, param) param->addListener(std::bind(&Class::onParameterEvent, this, std::placeholders::_1));

//Component Helpers
#define AddComponent(comp, Type, Enabled) comp = addComponent<Type ## Component>(Enabled);
#define AddDefaultComponentListener(comp) comp->addListener(std::bind(&Component::onChildComponentEvent, this, std::placeholders::_1));

#define DeclareComponentEventTypes(...) enum ComponentEventTypes { __VA_ARGS__, TYPES_MAX };
#define DeclareComponentEventNames(...) const String componentEventNames[TYPES_MAX] { __VA_ARGS__ };\
String getComponentEventName(uint8_t type) const override { return componentEventNames[type]; }

#define CheckCommand(cmd, num) checkCommand(command, cmd, numData, num)

// Preferences
#define GetConfig(sname, Class) SettingsComponent::instance->settings[name][sname].as<Class>()
#define GetStringConfig(sname) GetConfig(sname, String)
#define GetIntConfig(sname) GetConfig(sname, int)
#define GetFloatConfig(sname) GetConfig(sname, float)
#define GetBoolConfig(sname) GetConfig(sname, bool)

#define SetConfig(sname,val) SettingsComponent::instance->setConfig(name, sname, val, true);
#define SetConfigSave(sname,val) SettingsComponent::instance->setConfig(name, sname, val, false);




//Internal
#define PDerive(Class) ,public Class
#define DeclareComponentClass(ClassPrefix,...) class ClassPrefix ## Component :\
public Component \
__VA_ARGS__\
{\
public:
//

#define DeclareComponent(ClassPrefix,name,Derives) \
    DeclareComponentClass(ClassPrefix, Derives) \
    ClassPrefix ## Component(bool _enabled = true):Component(name, _enabled) {}\
    ~ClassPrefix ## Component(){}

#define DeclareComponentSingleton(ClassPrefix,name,Derives) \
    DeclareComponentClass(ClassPrefix, Derives)\
    DeclareSingleton(ClassPrefix ## Component) \
    ClassPrefix ## Component(bool _enabled = true):Component(name, _enabled) { InitSingleton() }\
    ~ClassPrefix ## Component(){ DeleteSingleton() }


#define EndDeclareComponent };


#define DeviceID RootComponent::instance->deviceID

#define SendParameterFeedback(param) CommunicationComponent::instance->sendParameterFeedback(this, param);
#define SendConfigFeedback(configName) CommunicationComponent::instance->sendConfigFeedback(this, configName, GetStringConfig(configName));

#define CommandCheck(cmd, Count) if(command == cmd) { if(numData < Count) { NDBG("setConfig needs 2 parameters, only " + String(numData) +" provided."); return false; } else {
#define ElifCommandCheck(cmd, Count) EndCommandCheck else CommandCheck(cmd, Count) 
#define EndCommandCheck }}