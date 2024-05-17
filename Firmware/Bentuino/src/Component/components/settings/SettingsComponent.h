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

#ifdef USE_POWER
DeclareIntParam(wakeUpButton, POWER_WAKEUP_BUTTON);
DeclareBoolParam(wakeUpState, POWER_WAKEUP_BUTTON_STATE);
#endif

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
    #ifdef USE_POWER
    CheckAndSetParam(wakeUpButton);
    CheckAndSetParam(wakeUpState);
    #endif
HandleSetParamInternalEnd;

FillSettingsInternalStart
    FillSettingsParam(deviceName);
    FillSettingsParam(deviceType);
    FillSettingsParam(propID);
    #ifdef USE_POWER
    FillSettingsParam(wakeUpButton);
    FillSettingsParam(wakeUpState);
    #endif
FillSettingsInternalEnd

    FillOSCQueryInternalStart
    FillOSCQueryTrigger(saveSettings);
    FillOSCQueryTrigger(clearSettings);
    FillOSCQueryIntParam(propID);
    FillOSCQueryStringParamReadOnly(macAddress);
    FillOSCQueryStringParam(deviceName);
    FillOSCQueryStringParam(deviceType);
    #ifdef USE_POWER
    FillOSCQueryIntParam(wakeUpButton);
    FillOSCQueryBoolParam(wakeUpState);
    #endif
FillOSCQueryInternalEnd

    EndDeclareComponent

    // Manager
