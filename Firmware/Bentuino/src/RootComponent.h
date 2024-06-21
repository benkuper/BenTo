#pragma once



DeclareComponentSingleton(Root, "root", )



// DeclareConfigParameter(deviceName, deviceID);
// DeclareConfigParameter(wakeUpButton, POWER_WAKEUP_BUTTON);
// DeclareConfigParameter(wakeUpState, POWER_WAKEUP_BUTTON_STATE);

// system
SettingsComponent settings;

CommunicationComponent comm;

#ifdef USE_DISPLAY
DisplayComponent display;
#endif

#ifdef USE_WIFI
WifiComponent wifi;
#endif

#ifdef USE_FILES
FilesComponent files;
#endif

#ifdef USE_SERVER
WebServerComponent server;
#endif

#ifdef USE_BATTERY
BatteryComponent battery;
#endif

#ifdef USE_SEQUENCE
SequenceComponent sequence;
#endif

#ifdef USE_LEDSTRIP
#ifdef USE_STREAMING
LedStreamReceiverComponent streamReceiver;
#endif
LedStripManagerComponent strips;
#endif

#ifdef USE_SCRIPT
ScriptComponent script;
#endif

#ifdef USE_IO
IOManagerComponent ios;
#ifdef USE_BUTTON
ButtonManagerComponent buttons;
#endif
#endif

#ifdef USE_BEHAVIOUR
BehaviourManagerComponent behaviours;
#endif

#ifdef USE_DUMMY
DummyManagerComponent dummies;
#endif

#ifdef USE_MOTION
MotionComponent motion;
#endif

#ifdef USE_SERVO
ServoComponent servo;
#endif

#ifdef USE_STEPPER
StepperComponent stepper;
#endif

#ifdef USE_DC_MOTOR
DCMotorComponent motor;
#endif

static bool availablePWMChannels[16];
int getFirstAvailablePWMChannel() const;

// Behaviour
Timer<5> timer;
long timeAtStart;
long timeAtShutdown;

bool initInternal(JsonObject o) override;
void updateInternal() override;

void shutdown();
void restart();

void powerdown();

void onChildComponentEvent(const ComponentEvent &e) override;
void childParamValueChanged(Component *caller, Component *comp, void *param);

bool handleCommandInternal(const String &command, var *data, int numData) override;

bool isShuttingDown() const { return timeAtShutdown > 0; }



HandleSetParamInternalStart
    CheckTrigger(shutdown);
CheckTrigger(restart);

HandleSetParamInternalEnd;

FillSettingsInternalStart

FillSettingsInternalEnd;

FillOSCQueryInternalStart
    FillOSCQueryTrigger(shutdown);
FillOSCQueryTrigger(restart);

FillOSCQueryInternalEnd

    EndDeclareComponent