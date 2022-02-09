#pragma once

#define TRAIL_MAX 20
#define NATIVE_STACK_SIZE (32 * 1024)

DeclareComponent(IMU, "imu", )

    Adafruit_BNO055 bno;
Parameter *isConnected;
Parameter *sendLevel;
Parameter* orientationSendRate;
Parameter *sdaPin;
Parameter *sclPin;

long timeSinceOrientationLastSent;

// IMU data
var calibration[4];
var orientation[3];
var accel[3];
var gyro[3];
var linearAccel[3];
var gravity[3];
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
DeclareComponentEventNames("orientation", "accel", "gyro", "linearAccel", "gravity", "throwState", "calibration", "activity", "debug", "projectedAngleClub");

EndDeclareComponent