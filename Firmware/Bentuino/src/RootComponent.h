#pragma once


DeclareComponentSingleton(Root, "root", )

const String deviceID = getDeviceID();
DeclareConfigParameter(deviceName, deviceID);
DeclareConfigParameter(wakeUpButton, POWER_WAKEUP_BUTTON);
DeclareConfigParameter(wakeUpState, POWER_WAKEUP_BUTTON_STATE);

// system
CommunicationComponent comm;
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

#ifdef USE_STREAM
LedStreamReceiverComponent streamReceiver;
#endif

#ifdef USE_LEDSTRIP
LedStripManagerComponent strips;
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