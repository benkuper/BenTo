#define DBG(text) SerialComponent::instance->send(text)
#define NDBG(text) SerialComponent::instance->send("["+name+"] "+text)

#define DeclareSingleton(Class) static Class * instance;
#define ImplementSingleton(Class) Class * Class::instance = NULL;
#define InitSingleton() instance = this;
#define DeleteSingleton() instance = NULL;

//Component Helpers

#define AddComponent(comp, Type) comp = addComponent<Type ## Component>();
#define AddDefaultComponentListener(comp) comp->addListener(std::bind(&Component::onChildComponentEvent, this, std::placeholders::_1));

#define DeclareEventTypes(...) enum EventTypes { __VA_ARGS__, TYPES_MAX };
#define DeclareEventNames(...) const String eventNames[TYPES_MAX] { __VA_ARGS__ };\
String getEventName(uint8_t type) const override { return eventNames[type]; }


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
    ClassPrefix ## Component():Component(name) {}\
    ~ClassPrefix ## Component(){}

#define DeclareComponentSingleton(ClassPrefix,name,Derives) \
    DeclareComponentClass(ClassPrefix, Derives)\
    DeclareSingleton(ClassPrefix ## Component) \
    ClassPrefix ## Component():Component(name) { InitSingleton() }\
    ~ClassPrefix ## Component(){ DeleteSingleton() }


#define EndDeclareComponent };


#define DeviceID RootComponent::instance->deviceID

#define SendParameterFeedback(param) CommunicationComponent::instance->sendParameterFeedback(this, param);
#define SendConfigFeedback(configName) CommunicationComponent::instance->sendConfigFeedback(this, configName, GetStringConfig(configName));

#define CommandCheck(cmd, Count) if(command == cmd) { if(numData < Count) { NDBG("setConfig needs 2 parameters, only " + String(numData) +" provided."); return false; } else {
#define ElifCommandCheck(cmd, Count) EndCommandCheck else CommandCheck(cmd, Count) 
#define EndCommandCheck }}