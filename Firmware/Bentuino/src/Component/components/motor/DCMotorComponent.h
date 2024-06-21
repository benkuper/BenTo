#pragma once

#ifndef DC_MOTOR_DEFAULT_EN_PIN
#define DC_MOTOR_DEFAULT_EN_PIN -1
#endif

#ifndef DC_MOTOR_DEFAULT_DIR1_PIN
#define DC_MOTOR_DEFAULT_DIR1_PIN -1
#endif

#ifndef DC_MOTOR_DEFAULT_DIR2_PIN
#define DC_MOTOR_DEFAULT_DIR2_PIN -1
#endif

DeclareComponent(DCMotor, "DCMotor", )

    int pwmChannel;

DeclareIntParam(enPin, DC_MOTOR_DEFAULT_EN_PIN);
DeclareIntParam(dir1Pin, DC_MOTOR_DEFAULT_DIR1_PIN);
DeclareIntParam(dir2Pin, DC_MOTOR_DEFAULT_DIR2_PIN);
DeclareFloatParam(speed, 0);

bool initInternal(JsonObject o) override;
void updateInternal() override;
void clearInternal() override;
void paramValueChangedInternal(void *param) override;

HandleSetParamInternalStart
    CheckAndSetParam(enPin);
CheckAndSetParam(dir1Pin);
CheckAndSetParam(dir2Pin);
CheckAndSetParam(speed);
HandleSetParamInternalEnd;

FillSettingsInternalStart
    FillSettingsParam(enPin);
FillSettingsParam(dir1Pin);
FillSettingsParam(dir2Pin);
FillSettingsParam(speed);
FillSettingsInternalEnd

    FillOSCQueryInternalStart
        FillOSCQueryIntParam(enPin);
FillOSCQueryIntParam(dir1Pin);
FillOSCQueryIntParam(dir2Pin);
FillOSCQueryRangeParam(speed, -1, 1);
FillOSCQueryInternalEnd

    EndDeclareComponent