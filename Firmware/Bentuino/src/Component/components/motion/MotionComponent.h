#pragma once

#define TRAIL_MAX 20
#define IMU_NATIVE_STACK_SIZE (32 * 1024)

DeclareComponent(Motion, "motion", )

    Adafruit_BNO055 bno;

DeclareBoolParam(connected, false);
DeclareIntParam(sendLevel, 0);
DeclareIntParam(orientationSendRate, 50);
DeclareIntParam(sdaPin, IMU_DEFAULT_SDA);
DeclareIntParam(sclPin, IMU_DEFAULT_SCL);

long timeSinceOrientationLastSent;

// IMU data
DeclareP3DParam(orientation, 0, 0, 0);
DeclareP3DParam(accel, 0, 0, 0);
DeclareP3DParam(gyro, 0, 0, 0);
DeclareP3DParam(linearAccel, 0, 0, 0);
DeclareFloatParam(orientationXOffset, 0);

DeclareIntParam(throwState, 0); // 0 = none, 1 = flat, 2 = single, 3 = double+, 4 = flat-front, 5 = loftie
DeclareFloatParam(activity, 0);
float prevActivity;
float debug[4];

// IMU Compute
DeclareP2DParam(flatThresholds, .8f, 2);
DeclareP3DParam(accelThresholds, .8f, 2, 4);
DeclareFloatParam(diffThreshold, 8);
DeclareFloatParam(semiFlatThreshold, 2);
DeclareFloatParam(loftieThreshold, 12);
DeclareFloatParam(singleThreshold, 25);

// Projected Angle
DeclareFloatParam(angleOffset, 0);
DeclareFloatParam(projectedAngle, 0);
DeclareFloatParam(xOnCalibration, 0);

// Threading
bool hasNewData;
bool imuLock;
bool shouldStopRead;

const String sendLevelOptions[3]{"None", "Orientation", "All"};
const String throwStateOptions[6]{"None", "Flat", "Single", "Double", "Flat Front", "Loftie"};

bool initInternal(JsonObject o) override;
void updateInternal() override;
void clearInternal() override;

void startIMUTask();

static void readIMUStatic(void *);

bool setupBNO();
void readIMU();
void sendCalibrationStatus();
void computeThrow();
void computeActivity();
void computeProjectedAngle();

void setOrientationXOffset(float offset);
void setProjectAngleOffset(float yaw, float angle);

void onEnabledChanged() override;

bool handleCommandInternal(const String &command, var *data, int numData) override;

DeclareComponentEventTypes(OrientationUpdate, AccelUpdate, GyroUpdate, LinearAccelUpdate, ThrowState, CalibrationStatus, ActivityUpdate, Debug, ProjectedAngleUpdate);
DeclareComponentEventNames("orientation", "accel", "gyro", "linearAccel", "throwState", "calibration", "activity", "debug", "projectedAngle");

#ifdef USE_SCRIPT
LinkScriptFunctionsStart
    LinkScriptFunction(MotionComponent, getOrientation, f, i);
LinkScriptFunction(MotionComponent, getYaw, f, );
LinkScriptFunction(MotionComponent, getPitch, f, );
LinkScriptFunction(MotionComponent, getRoll, f, );
LinkScriptFunction(MotionComponent, getProjectedAngle, f, );
LinkScriptFunction(MotionComponent, setProjectedAngleOffset, v, ff);
LinkScriptFunction(MotionComponent, getActivity, f, );
LinkScriptFunction(MotionComponent, getThrowState, i, );
LinkScriptFunctionsEnd

DeclareScriptFunctionReturn1(MotionComponent, getOrientation, float, uint32_t)
{
    return arg1 >= 3 ? 0.0f : orientation[arg1];
}
DeclareScriptFunctionReturn0(MotionComponent, getYaw, float) { return orientation[0]; }
DeclareScriptFunctionReturn0(MotionComponent, getPitch, float) { return orientation[1]; }
DeclareScriptFunctionReturn0(MotionComponent, getRoll, float) { return orientation[2]; }
DeclareScriptFunctionReturn0(MotionComponent, getProjectedAngle, float) { return projectedAngle; }
DeclareScriptFunctionVoid2(MotionComponent, setProjectedAngleOffset, float, float) { setProjectAngleOffset(arg1, arg2); }
DeclareScriptFunctionReturn0(MotionComponent, getActivity, float) { return activity; }
DeclareScriptFunctionReturn0(MotionComponent, getThrowState, uint32_t) { return throwState; }
#endif

// CheckFeedbackParamInternalStart
//     CheckAndSendParamFeedback(orientation);
// CheckAndSendParamFeedback(accel);
// CheckAndSendParamFeedback(gyro);
// CheckAndSendParamFeedback(linearAccel);
// CheckAndSendParamFeedback(gravity);
// CheckAndSendParamFeedback(projectedAngle);
// CheckFeedbackParamInternalEnd;

HandleSetParamInternalStart
    CheckAndSetEnumParam(sendLevel, sendLevelOptions, 3);
CheckAndSetParam(orientationSendRate);
CheckAndSetParam(sdaPin);
CheckAndSetParam(sclPin);
CheckAndSetParam(orientationXOffset);
CheckAndSetParam(flatThresholds);
CheckAndSetParam(accelThresholds);
CheckAndSetParam(diffThreshold);
CheckAndSetParam(semiFlatThreshold);
CheckAndSetParam(loftieThreshold);
CheckAndSetParam(singleThreshold);
CheckAndSetParam(angleOffset);
CheckAndSetParam(xOnCalibration);
HandleSetParamInternalEnd;

FillSettingsInternalStart
    FillSettingsParam(sendLevel);
FillSettingsParam(orientationSendRate);
FillSettingsParam(sdaPin);
FillSettingsParam(sclPin);
FillSettingsParam(orientationXOffset);
FillSettingsParam2(flatThresholds);
FillSettingsParam3(accelThresholds);
FillSettingsParam(diffThreshold);
FillSettingsParam(semiFlatThreshold);
FillSettingsParam(loftieThreshold);
FillSettingsParam(singleThreshold);
FillSettingsParam(angleOffset);
FillSettingsParam(xOnCalibration);
FillSettingsInternalEnd;

FillOSCQueryInternalStart
    FillOSCQueryBoolParamReadOnly(connected);
FillOSCQueryEnumParam(sendLevel, sendLevelOptions, 3);
FillOSCQueryIntParam(orientationSendRate);
FillOSCQueryIntParam(sdaPin);
FillOSCQueryIntParam(sclPin);
FillOSCQueryP3DRangeParamReadOnly(orientation, -180, 180, -90, 90, -180, 180);
FillOSCQueryP3DParamReadOnly(accel);
FillOSCQueryP3DParamReadOnly(gyro);
FillOSCQueryP3DParamReadOnly(linearAccel);
FillOSCQueryFloatParam(orientationXOffset);
FillOSCQueryEnumParamReadOnly(throwState, throwStateOptions, 6);
FillOSCQueryFloatParamReadOnly(activity);
FillOSCQueryP2DParam(flatThresholds);
FillOSCQueryP3DParam(accelThresholds);
FillOSCQueryFloatParam(diffThreshold);
FillOSCQueryFloatParam(semiFlatThreshold);
FillOSCQueryFloatParam(loftieThreshold);
FillOSCQueryFloatParam(singleThreshold);
FillOSCQueryFloatParam(angleOffset);
FillOSCQueryFloatParamReadOnly(projectedAngle);
FillOSCQueryFloatParam(xOnCalibration);
FillOSCQueryInternalEnd
    EndDeclareComponent