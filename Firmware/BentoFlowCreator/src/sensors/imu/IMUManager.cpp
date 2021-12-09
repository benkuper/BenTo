#include "IMUManager.h"

const String IMUEvent::eventNames[IMUEvent::TYPES_MAX]{"orientation", "accel", "gyro", "linearAccel", "gravity", "throwState", "calibration", "activity", "debug", "projectedAngleClub"};
IMUManager *IMUManager::instance = NULL;

IMUManager::IMUManager() : Component("imu"),
#ifdef HAS_IMU
                           bno(55),
#endif
                           isConnected(false),
                           isEnabled(false),
                           sendLevel(1),
                           orientationSendTime(20), // 50fps
                           timeSinceOrientationLastSent(0),
                           throwState(0)
#ifdef IMU_READ_ASYNC
                           ,
                           hasNewData(false), shouldStopRead(false), imuLock(false)
#endif
{

  instance = this;

#ifdef HAS_IMU
  bno.setMode(Adafruit_BNO055::OPERATION_MODE_CONFIG);
  bno.setAxisRemap(Adafruit_BNO055::IMU_REMAP_CONFIG);
  bno.setAxisSign(Adafruit_BNO055::IMU_REMAP_SIGN);
  bno.setMode(Adafruit_BNO055::OPERATION_MODE_NDOF);
  bno.setExtCrystalUse(true);
#endif

  accelThresholds[0] = .8f;
  accelThresholds[1] = 2;
  accelThresholds[2] = 4;

  diffThreshold = 8;

  flatThresholds[0] = .8f;
  flatThresholds[1] = 2;

  semiFlatThreshold = 2;
  loftieThreshold = 12;
  singleThreshold = 25;

  activity = .0;
  prevActivity = .0;
  orientationXOffset = .0;

  angleOffset = .0f;
}

IMUManager::~IMUManager()
{
}

void IMUManager::init()
{
#ifdef HAS_IMU

  NDBG("Init");
  if (isConnected)
    return;

  Wire.begin(SDA_PIN, SCL_PIN);

  if (!bno.begin())
  {
    NDBG("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    isConnected = false;
    return;
  }

  bno.setMode(Adafruit_BNO055::OPERATION_MODE_CONFIG);
  bno.setAxisRemap(IMU_REMAP_CONFIG);
  bno.setAxisSign(IMU_REMAP_SIGN);
  bno.setMode(Adafruit_BNO055::OPERATION_MODE_NDOF);

  //bno.setExtCrystalUse(true);
  bno.enterNormalMode();

  isConnected = true;
  NDBG("Imu is connected.");

#ifdef USE_PREFERENCES
  prefs.begin(name.c_str());
  accelThresholds[0] = prefs.getFloat("throwMin", accelThresholds[0]);
  accelThresholds[1] = prefs.getFloat("throwMax", accelThresholds[1]);
  accelThresholds[1] = prefs.getFloat("throwFast", accelThresholds[2]);
  diffThreshold = prefs.getFloat("diff", diffThreshold);
  flatThresholds[0] = prefs.getFloat("flatMin", flatThresholds[0]);
  flatThresholds[1] = prefs.getFloat("flatMax", flatThresholds[1]);

  semiFlatThreshold = prefs.getFloat("semiFlat", semiFlatThreshold);
  loftieThreshold = prefs.getFloat("loftie", loftieThreshold);
  singleThreshold = prefs.getFloat("single", singleThreshold);

  orientationXOffset = prefs.getFloat("xOffset", orientationXOffset);
  prefs.end();
#endif

#ifdef IMU_READ_ASYNC
    DBG("IMU Task Create");
    xTaskCreate(&IMUManager::readIMUStatic, "imu", NATIVE_STACK_SIZE, NULL, 1, NULL);
#endif

  DBG("IMU CONFIG: ");
  DBG("flat threshold " + String(flatThresholds[0]) + " " + String(flatThresholds[1]));
  DBG("accel threshold " + String(accelThresholds[0]) + " " + String(accelThresholds[1]));
  DBG("diff threshold " + String(diffThreshold));
  DBG("semiFlat threshold " + String(semiFlatThreshold));
  DBG("loftie threshold " + String(loftieThreshold));
  DBG("single threshold " + String(singleThreshold));
  DBG("orientation x offset " + String(orientationXOffset));

#endif
}

void IMUManager::update()
{
#ifdef HAS_IMU
  if (!isEnabled || !isConnected)
    return;

#ifdef IMU_READ_ASYNC
  if (!hasNewData)
    return;
  hasNewData = false;
  imuLock = true;
#else
  readIMU();
#endif

  long curTime = millis();
  if (curTime > timeSinceOrientationLastSent + orientationSendTime)
  {
   // TSTART()
    if (sendLevel >= 1)
    {

      sendEvent(IMUEvent(IMUEvent::OrientationUpdate, orientation, 3));
      if (sendLevel >= 2)
      {
        sendEvent(IMUEvent(IMUEvent::AccelUpdate, accel, 3));
        sendEvent(IMUEvent(IMUEvent::LinearAccelUpdate, linearAccel, 3));
        sendEvent(IMUEvent(IMUEvent::GyroUpdate, gyro, 3));
        sendEvent(IMUEvent(IMUEvent::ActivityUpdate));
        sendEvent(IMUEvent(IMUEvent::ProjectedAngleUpdate));
        // sendEvent(IMUEvent(IMUEvent::Gravity, gravity, 3));
      }
    }

    timeSinceOrientationLastSent = curTime;
  //  TFINISH("Send")
  }

#ifdef IMU_READ_ASYNC
  imuLock = false;
#endif

#endif
}

#ifdef IMU_READ_ASYNC
void IMUManager::readIMUStatic(void *)
{
  DBG("Thread IMU Start");
  while (!instance->shouldStopRead)
  {
    instance->readIMU();
    delay(5);
  }

  DBG("Thread IMU Finish");

  vTaskDelete(NULL);

  DBG("Task deleted");
}
#endif

void IMUManager::readIMU()
{
  if (!isEnabled)
    return;
#ifdef HAS_IMU

#ifdef IMU_READ_ASYNC
  if (imuLock)
    return;
#endif

  //DBG("Read IMU");

  imu::Quaternion q = bno.getQuat();
  q.normalize();

  // float temp = q.x();  q.x() = -q.y();  q.y() = temp;
  // q.z() = -q.z();

  imu::Vector<3> euler = q.toEuler();
  orientation[0] = fmod(((euler.x() * 180 / PI) + orientationXOffset + 180.0f*5), 360.0f) - 180.0f;
  orientation[1] = euler.y() * 180 / PI; // Pitch
  orientation[2] = euler.z() * 180 / PI; // Roll
  

  imu::Vector<3> acc = bno.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
  accel[0] = acc.x();
  accel[1] = acc.y();
  accel[2] = acc.z();

  imu::Vector<3> laccel = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
  linearAccel[0] = laccel.x();
  linearAccel[1] = laccel.y();
  linearAccel[2] = laccel.z();

  imu::Vector<3> gyr = bno.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);
  gyro[0] = gyr.x();
  gyro[1] = gyr.y();
  gyro[2] = gyr.z();

  // imu::Vector<3> grav = bno.getVector(Adafruit_BNO055::VECTOR_GRAVITY);
  // gravity[0] = grav.x();
  // gravity[1] = grav.y();
  // gravity[2] = grav.z();

  computeThrow();
  computeActivity();
  computeProjectedAngle();

#ifdef IMU_READ_ASYNC
  hasNewData = true;
#endif

#endif
}

void IMUManager::computeProjectedAngle() {
  float eulerRadians[3];
  float lookAt[3];
  float result;

  //Recalculate x orientation for the projected angle, based on xOnCalibration
  float xOrientation = orientation[0];
  float newX = 0;
  if (xOnCalibration < 0) {
    if (xOrientation > xOnCalibration) {
      if (xOrientation < 0) {
        newX = (xOnCalibration * -1) - (xOrientation * -1);
      } else {
        if (xOrientation + (xOnCalibration * -1) > 180.0f) {
          newX = (360.0f - xOrientation - (xOnCalibration * -1)) * -1;
        } else {
          newX = xOrientation + (xOnCalibration * -1);
        }
      }
    } else {
      newX = (xOnCalibration * -1) - (xOrientation * -1);
    }
  } else {
    if (xOrientation > xOnCalibration) {
      newX = xOrientation - xOnCalibration;
    } else {
      if ((xOrientation - xOnCalibration) < -180) {
        newX = (180.0f - xOnCalibration) + (180.0f - (xOrientation * -1));
      } else {
        newX = xOrientation - xOnCalibration;
      }
    }
  }

  eulerRadians[0] = newX * PI / 180.0f;
  eulerRadians[1] = orientation[1] * PI / 180.0f;
  eulerRadians[2] = orientation[2] * PI / 180.0f;

  lookAt[0] = cos(eulerRadians[1]) * sin(eulerRadians[0]);
  lookAt[1] = sin(eulerRadians[1]);
  lookAt[2] = cos(eulerRadians[1]) * cos(eulerRadians[0]);

  result = atan(lookAt[1] / lookAt[2]) - PI / 2.0f;

  if (lookAt[2] > 0.0f) {
    result = result + PI;
  }

  result = (result / PI) * 0.5f + 0.5f;
	result = fmod((result + angleOffset), 1.0f);

  projectedAngle = result;

  // DBG("Projected Angle: " + String(projectedAngle));
}


void IMUManager::computeThrow()
{
#ifdef HAS_IMU
  float maxAccelYZ = max(abs(accel[1]), abs(accel[2]));
  float maxAccel = max(maxAccelYZ, abs(accel[0]));
  float maxLinearAccel = max(max(abs(linearAccel[0]), abs(linearAccel[1])), abs(linearAccel[2]));
  float accLinearDiff = abs(maxAccelYZ - maxLinearAccel);

  bool curIsFlat = throwState == 1;
  float flatThresh = curIsFlat ? flatThresholds[1] : flatThresholds[0];
  bool isFlatting = maxAccel < flatThresh;
  bool isFastSpin = false;

  int newState = 0;
  if (isFlatting)
  {
    newState = 1; // flat
  }
  else
  {
    bool curIsThrowing = throwState > 1;
    float throwThresh = curIsThrowing ? accelThresholds[1] : accelThresholds[0];
		throwThresh = isFastSpin ? throwThresh : accelThresholds[2];

    bool accelCheck = maxAccelYZ < throwThresh;

    bool isThrowing = false;
    
    if (curIsThrowing)
    {
      isThrowing = accelCheck;
    }
    else
    {
      bool diffCheck = accLinearDiff > diffThreshold;
      isThrowing = accelCheck && diffCheck;
    }

    float throwPower = abs(accel[0]);
    if (isThrowing)
    {
      if (throwPower < semiFlatThreshold)
        newState = 4;
      else if (throwPower < loftieThreshold)
        newState = 5;
      else if (throwPower < singleThreshold)
        newState = 2;
      else
        newState = 3; // double
    }
  }

  // NDBG(String(throwState));

  if (newState != throwState)
  {
    throwState = newState;
    sendEvent(IMUEvent(IMUEvent::ThrowState));
  }

#endif
}


void IMUManager::computeActivity()
{
  float maxLinearAccel = max(max(abs(linearAccel[0]), abs(linearAccel[1])), abs(linearAccel[2]));
  maxLinearAccel = 	(((maxLinearAccel - 0) * (1 - 0)) / (40 - 0)) + 0; // remap to 0..1 range
  maxLinearAccel = min(maxLinearAccel, (float)1.0);

  activity = prevActivity + (maxLinearAccel - prevActivity) * 0.1;
  prevActivity = activity;
}


void IMUManager::sendCalibrationStatus() {
  uint8_t system, gyro, accel, mag;
  system = gyro = accel = mag = 0;
  bno.getCalibration(&system, &gyro, &accel, &mag);

  calibration[0] = (float)system;
  calibration[1] = (float)gyro;
  calibration[2] = (float)accel;
  calibration[3] = (float)mag;

  sendEvent(IMUEvent(IMUEvent::CalibrationStatus, calibration, 4));
}


void IMUManager::setEnabled(bool value)
{
  if (isEnabled == value)
    return;

  isEnabled = value;
}

void IMUManager::setOrientationXOffset(float offset = 0.0f)
{
  orientationXOffset = offset;
}

void IMUManager::setProjectAngleOffset(float yaw = 0.0f, float angle = 0.0f)
{
  angleOffset = angle;
  xOnCalibration = yaw;
}

void IMUManager::shutdown()
{
  setEnabled(false);
  #ifdef IMU_READ_ASYNC
    shouldStopRead = true;
    #endif
}

bool IMUManager::handleCommand(String command, var *data, int numData)
{
#ifdef HAS_IMU
  if (checkCommand(command, "enabled", numData, 1))
  {
    setEnabled(data[0].intValue());
    return true;
  }
  if (checkCommand(command, "sendLevel", numData, 1))
  {
    sendLevel = data[0].intValue();
    return true;
  }
  else if (checkCommand(command, "updateRate", numData, 1))
  {
    orientationSendTime = 1000 / data[0].intValue();
    return true;
  }
  else if (checkCommand(command, "calibrationStatus", numData, 0))
  {
    sendCalibrationStatus();
    return true;
  }
  else if (checkCommand(command, "accelThresholds", numData, 3))
  {
    accelThresholds[0] = data[0].floatValue();
    accelThresholds[1] = data[1].floatValue();
    accelThresholds[2] = data[2].floatValue();
    prefs.begin(name.c_str());
    prefs.putFloat("throwMin", accelThresholds[0]);
    prefs.putFloat("throwMax", accelThresholds[1]);
    prefs.putFloat("throwFast", accelThresholds[2]);
    prefs.end();
    return true;
  }
  else if (checkCommand(command, "diffThreshold", numData, 1))
  {
    diffThreshold = data[0].floatValue();
    prefs.begin(name.c_str());
    prefs.putFloat("diff", diffThreshold);
    prefs.end();
    return true;
  }
  else if (checkCommand(command, "flatThresholds", numData, 2))
  {
    flatThresholds[0] = data[0].floatValue();
    flatThresholds[1] = data[1].floatValue();
    prefs.begin(name.c_str());
    prefs.putFloat("flatMin", flatThresholds[0]);
    prefs.putFloat("flatMax", flatThresholds[1]);
    prefs.end();
    return true;
  }
  else if (checkCommand(command, "semiFlatThreshold", numData, 1))
  {
    semiFlatThreshold = data[0].floatValue();
    prefs.begin(name.c_str());
    prefs.putFloat("semiFlat", semiFlatThreshold);
    prefs.end();
    return true;
  }
  else if (checkCommand(command, "loftieThreshold", numData, 1))
  {
    loftieThreshold = data[0].floatValue();
    prefs.begin(name.c_str());
    prefs.putFloat("loftie", loftieThreshold);
    prefs.end();
    return true;
  }
  else if (checkCommand(command, "singleThreshold", numData, 1))
  {
    singleThreshold = data[0].floatValue();
    prefs.begin(name.c_str());
    prefs.putFloat("single", singleThreshold);
    prefs.end();
    return true;
  }
  else if (checkCommand(command, "orientationXOffset", numData, 1))
  {
    orientationXOffset = data[0].floatValue();
    prefs.begin(name.c_str());
    prefs.putFloat("xOffset", orientationXOffset);
    prefs.end();
    return true;
  }
#endif
  return false;
}
