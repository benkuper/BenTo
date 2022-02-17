//#define NUM_SYSTEM_COMPONENTS 6
class RootComponent : public Component
{
public:
    RootComponent() : Component(Type_Root, "root") { instance = this; }
    ~RootComponent() {}

    DeclareSingleton(RootComponent);

    String deviceID;
    
    // system
    CommunicationComponent comm;
    WifiComponent wifi;
    FilesComponent files;
    WebServerComponent server;
    // ScriptComponent *script;

    BatteryComponent battery;
    SequenceComponent sequence;

#if NUM_BUTTONS > 0
    ButtonComponent buttons[NUM_BUTTONS];
#endif

#if NUM_IMUS > 0
    IMUComponent imus[NUM_IMUS];
#endif

#if NUM_STRIPS
    LedStreamReceiverComponent streamReceiver;
    LedStripComponent strips[NUM_STRIPS];
#endif

#if NUM_SERVOS
    ServoComponent servo[NUM_SERVOS];
#endif

#if NUM_STEPPERS > 0
    StepperComponent stepper[NUM_STEPPERS];
#endif

#if NUM_IOS > 0
    IOComponent ios[NUM_IOS];
#endif

    // Behaviour
    Timer<5> timer;
    long timeAtShutdown;

    bool initInternal(JsonObject o) override;
    void updateInternal() override;

    void shutdown();
    void restart();

    void powerdown();

    void saveSettings();

    void onChildComponentEvent(const ComponentEvent &e) override;

    bool handleCommandInternal(const String &command, var *data, int numData) override;

    bool isShuttingDown() const { return timeAtShutdown > 0; }
    // const Component *systemComponents[NUM_SYSTEM_COMPONENTS]{comm, wifi, files, server, sequence, battery};
};