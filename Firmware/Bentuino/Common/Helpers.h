#define DBG(text) SerialComponent::instance->send(text);
#define NDBG(text) SerialComponent::instance->send("["+name+"] "+text);

#define DeclareSingleton(Class) static Class * instance;
#define ImplementSingleton(Class) Class * Class::instance = NULL;
#define InitSingleton() instance = this;
#define DeleteSingleton() instance = NULL;

//Component Helpers

#define AddComponent(v, Type) addComponent<Type ## Component>();
#define AddDefaultComponentListener(comp) comp->addListener(std::bind(&Component::onChildComponentEvent, this, std::placeholders::_1));

#define DeclareEventTypes(...) enum EventTypes { __VA_ARGS__, TYPES_MAX };
#define DeclareEventNames(...) const String eventNames[TYPES_MAX] { __VA_ARGS__ };\
String getEventName(uint8_t type) const override { return eventNames[type]; }


// Preferences
#define LoadFloat(name,defaultVal) SettingsComponent::instance->getFloat(this,name,defaultVal) 
#define LoadInt(name,defaultVal) SettingsComponent::instance->getInt(this,name,defaultVal) 
#define LoadString(name,defaultVal) SettingsComponent::instance->getString(this,name,defaultVal) 
#define LoadBool(name,defaultVal) SettingsComponent::instance->getBool(this,name,defaultVal)

#define SaveFloat(name) SettingsComponent::instance->getFloat(this,name) 
#define SaveInt(name) SettingsComponent::instance->getInt(this,name) 
#define SaveString(name) SettingsComponent::instance->getString(this,name) 
#define SaveBool(name) SettingsComponent::instance->getBool(this,name) 

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
