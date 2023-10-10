#pragma once


DeclareComponentSingleton(Root, "root", )

const String deviceID = getDeviceID();
DeclareConfigParameter(deviceName, deviceID);
DeclareConfigParameter(wakeUpButton, POWER_WAKEUP_BUTTON);
DeclareConfigParameter(wakeUpState, POWER_WAKEUP_BUTTON_STATE);

// system
CommunicationComponent comm;
WifiComponent wifi;
FilesComponent files;
WebServerComponent server;
// ScriptComponent script;
BatteryComponent battery;
SequenceComponent sequence;
LedStreamReceiverComponent streamReceiver;

LedStripManagerComponent strips;
ButtonManagerComponent buttons;
IOManagerComponent ios;
BehaviourManagerComponent behaviours;

#ifdef USE_IMU
IMUComponent imu;
#endif

#ifdef USE_SERVER
ServoComponent *servo;
#endif

#ifdef USE_STEPPER
StepperComponent *stepper;
#endif

// Needs a single structure

// Behaviour
Timer<5> timer;
long timeAtShutdown;

bool initInternal(JsonObject o) override;
void updateInternal() override;

void shutdown();
void restart();

void powerdown();

void saveSettings();
void clearSettings();

void onChildComponentEvent(const ComponentEvent &e) override;

bool handleCommandInternal(const String &command, var *data, int numData) override;

bool isShuttingDown() const { return timeAtShutdown > 0; }

String getDeviceID() const;

// const Component *systemComponents[NUM_SYSTEM_COMPONENTS]{comm, wifi, files, server, sequence, battery};

EndDeclareComponent