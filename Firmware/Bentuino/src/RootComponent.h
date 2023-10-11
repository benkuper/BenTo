#pragma once

#ifndef DEVICE_TYPE
#define DEVICE_TYPE "Bento"
#endif

DeclareComponentSingleton(Root, "root", )

    const String deviceID = getDeviceID();

DeclareStringParam(id, deviceID);
DeclareStringParam(deviceName, deviceID);
DeclareStringParam(deviceType, DEVICE_TYPE);

DeclareIntParam(wakeUpButton, POWER_WAKEUP_BUTTON);
DeclareBoolParam(wakeUpState, POWER_WAKEUP_BUTTON_STATE);

// DeclareConfigParameter(deviceName, deviceID);
// DeclareConfigParameter(wakeUpButton, POWER_WAKEUP_BUTTON);
// DeclareConfigParameter(wakeUpState, POWER_WAKEUP_BUTTON_STATE);

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

#ifdef USE_STREAMING
LedStreamReceiverComponent streamReceiver;
#endif

#ifdef USE_LEDSTRIP
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

#ifdef USE_IMU
IMUComponent imu;
#endif

#ifdef USE_SERVO
ServoComponent servo;
#endif

#ifdef USE_STEPPER
StepperComponent stepper;
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

HandleSetParamInternalStart
    CheckAndSetParam(deviceName);
CheckAndSetParam(deviceType);
CheckAndSetParam(wakeUpButton);
CheckAndSetParam(wakeUpState);
HandleSetParamInternalEnd;

FillSettingsInternalStart
    FillSettingsParam(deviceName);
FillSettingsParam(deviceType);
FillSettingsParam(wakeUpButton);
FillSettingsParam(wakeUpState);
FillSettingsInternalEnd;

FillOSCQueryInternalStart
    FillOSCQueryStringParamReadOnly(id);
FillOSCQueryStringParam(deviceName);
FillOSCQueryStringParam(deviceType);
FillOSCQueryIntParam(wakeUpButton);
FillOSCQueryBoolParam(wakeUpState);
FillOSCQueryInternalEnd

    EndDeclareComponent