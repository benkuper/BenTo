#pragma once

#ifndef DEVICE_TYPE
#define DEVICE_TYPE "Bento"
#endif

DeclareComponentSingleton(Settings, "settings", )

    const String deviceID = getDeviceID();

DeclareIntParam(propID, 0);
DeclareStringParam(macAddress, deviceID);
DeclareStringParam(deviceName, DEVICE_TYPE);
DeclareStringParam(deviceType, DEVICE_TYPE);

DeclareIntParam(wakeUpButton, POWER_WAKEUP_BUTTON);
DeclareBoolParam(wakeUpState, POWER_WAKEUP_BUTTON_STATE);

bool initInternal(JsonObject o) override;

bool handleCommandInternal(const String &command, var *data, int numData) override;

void saveSettings();
void clearSettings();

String getDeviceID() const;



HandleSetParamInternalStart
    CheckTrigger(saveSettings);
    CheckTrigger(clearSettings);
    CheckAndSetParam(deviceName);
    CheckAndSetParam(deviceType);
    CheckAndSetParam(propID);
    CheckAndSetParam(wakeUpButton);
    CheckAndSetParam(wakeUpState);
HandleSetParamInternalEnd;

FillSettingsInternalStart
    FillSettingsParam(deviceName);
    FillSettingsParam(deviceType);
    FillSettingsParam(propID);
    FillSettingsParam(wakeUpButton);
    FillSettingsParam(wakeUpState);
FillSettingsInternalEnd

    FillOSCQueryInternalStart
    FillOSCQueryTrigger(saveSettings);
    FillOSCQueryTrigger(clearSettings);
    FillOSCQueryIntParam(propID);
    FillOSCQueryStringParamReadOnly(macAddress);
    FillOSCQueryStringParam(deviceName);
    FillOSCQueryStringParam(deviceType);
    FillOSCQueryIntParam(wakeUpButton);
    FillOSCQueryBoolParam(wakeUpState);
FillOSCQueryInternalEnd

    EndDeclareComponent

    // Manager
