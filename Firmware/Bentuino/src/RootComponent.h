#pragma once

DeclareComponentSingleton(Root, "root", )

const String deviceID = getDeviceID();
Parameter deviceName{"deviceName", deviceID, var(), var(), true};

// system
CommunicationComponent comm;
WifiComponent wifi;
FilesComponent files;
WebServerComponent server;
ScriptComponent script;

BatteryComponent battery;
SequenceComponent sequence;

LedStreamReceiverComponent streamReceiver;

// users
LedStripComponent *strip;
ButtonComponent *button;
IMUComponent *imu;
ServoComponent *servo;
StepperComponent *stepper;

// IOComponent * ioComponents[16];

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