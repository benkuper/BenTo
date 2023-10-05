#pragma once

#define TRAIL_MAX 20
#define IMU_NATIVE_STACK_SIZE (32 * 1024)

DeclareComponent(IMU, "imu", )

    Adafruit_BNO055 bno;
Parameter isConnected { "isConnected", false };
Parameter sendLevel { "sendLevel", 0, 0, 3, true};
Parameter orientationSendRate {"orientationSendRate", 50, var(), var(), true};
Parameter sdaPin {"sdaPin", IMU_DEFAULT_SDA, var(), var(), true};
Parameter sclPin {"sclPin", IMU_DEFAULT_SCL, var(), var(), true};

long timeSinceOrientationLastSent;

// IMU data
float orientation[3];
float accel[3];
float gyro[3];
float linearAccel[3];
float gravity[3];
float orientationXOffset;

int throwState; // 0 = none, 1 = flat, 2 = single, 3 = double+, 4 = flat-front, 5 = loftie
float activity;
float prevActivity;
float debug[4];

// IMU Compute
float flatThresholds[2];
float accelThresholds[3];
float diffThreshold;
float semiFlatThreshold;
float loftieThreshold;
float singleThreshold;

// Projected Angle
float angleOffset;
float projectedAngle;
float xOnCalibration;

//Threading
bool hasNewData;
bool imuLock;
bool shouldStopRead;
bool imuIsInit;

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

DeclareComponentEventTypes(OrientationUpdate, AccelUpdate, GyroUpdate, LinearAccelUpdate, Gravity, ThrowState, CalibrationStatus, ActivityUpdate, Debug, ProjectedAngleUpdate);
DeclareComponentEventNames("orientation", "accel", "gyro", "linearAccel", "gravity", "throwState", "calibration", "activity", "debug", "projectedAngle");

LinkScriptFunctionsStart
LinkScriptFunction(IMUComponent, getOrientation,f, i);
LinkScriptFunction(IMUComponent, getYaw,f,);
LinkScriptFunction(IMUComponent, getPitch,f,);
LinkScriptFunction(IMUComponent, getRoll,f,);
LinkScriptFunction(IMUComponent, getProjectedAngle,f,);
LinkScriptFunction(IMUComponent, setProjectedAngleOffset,v,ff);
LinkScriptFunction(IMUComponent, getActivity,f,);
LinkScriptFunction(IMUComponent, getThrowState,i,);
LinkScriptFunctionsEnd

DeclareScriptFunctionReturn1(IMUComponent, getOrientation,float, uint32_t) { return arg1 >= 3 ? 0.0f : orientation[arg1]; }
DeclareScriptFunctionReturn0(IMUComponent, getYaw,float)  { return orientation[0]; }
DeclareScriptFunctionReturn0(IMUComponent, getPitch,float)  { return orientation[1]; }
DeclareScriptFunctionReturn0(IMUComponent, getRoll,float)  { return orientation[2]; }
DeclareScriptFunctionReturn0(IMUComponent, getProjectedAngle,float)  { return projectedAngle; }
DeclareScriptFunctionVoid2  (IMUComponent, setProjectedAngleOffset,float, float) { setProjectAngleOffset(arg1, arg2); }
DeclareScriptFunctionReturn0(IMUComponent, getActivity,float)  { return activity; }
DeclareScriptFunctionReturn0(IMUComponent, getThrowState,uint32_t)  { return throwState; }
    
EndDeclareComponent