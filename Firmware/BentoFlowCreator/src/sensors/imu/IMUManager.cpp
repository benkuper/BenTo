#include "IMUManager.h"

const String IMUEvent::eventNames[IMUEvent::TYPES_MAX]{"orientation", "accel", "gyro", "linearAccel", "gravity", "throwState", "calibration"};

IMUManager::IMUManager() : Component("imu"),
#ifdef HAS_IMU
                           bno(55),
#endif
                           isConnected(false),
                           isEnabled(false),
                           sendLevel(1),
                           orientationSendTime(20), //50fps
                           timeSinceOrientationLastSent(0),
                           throwState(0)
{
#ifdef HAS_IMU
  bno.setMode(Adafruit_BNO055::OPERATION_MODE_CONFIG);
  bno.setAxisRemap(Adafruit_BNO055::IMU_REMAP_CONFIG);
  bno.setAxisSign(Adafruit_BNO055::IMU_REMAP_SIGN);
  bno.setMode(Adafruit_BNO055::OPERATION_MODE_NDOF);
#endif

  accelThresholds[0] = .9f;
  accelThresholds[1] = 2;

  diffThreshold = 10;

  flatThresholds[0] = .8f;
  flatThresholds[1] = 2;

  semiFlatThreshold = 2;
  loftieThreshold = 12;
  singleThreshold = 25;
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

  bno.setExtCrystalUse(true);

  isConnected = true;
  NDBG("Imu is connected.");

#ifdef USE_PREFERENCES
  prefs.begin(name.c_str());
  accelThresholds[0] = prefs.getFloat("throwMin", accelThresholds[0]);
  accelThresholds[1] = prefs.getFloat("throwMax", accelThresholds[1]);
  diffThreshold = prefs.getFloat("diff", diffThreshold);
  flatThresholds[0] = prefs.getFloat("flatMin", flatThresholds[0]);
  flatThresholds[1] = prefs.getFloat("flatMax", flatThresholds[1]);

  semiFlatThreshold = prefs.getFloat("semiFlat", semiFlatThreshold);
  loftieThreshold = prefs.getFloat("loftie", loftieThreshold);
  singleThreshold = prefs.getFloat("single", singleThreshold);
  prefs.end();
#endif

#endif
}

void IMUManager::update()
{
  if (!isEnabled || !isConnected)
    return;

#ifdef HAS_IMU
  imu::Quaternion q = bno.getQuat();
  q.normalize();

  //float temp = q.x();  q.x() = -q.y();  q.y() = temp;
  //q.z() = -q.z();
  imu::Vector<3> euler = q.toEuler();
  orientation[0] = euler.x() * 180 / PI; //Yaw
  orientation[1] = euler.y() * 180 / PI; //Pitch
  orientation[2] = euler.z() * 180 / PI; //Roll

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

  long curTime = millis();
  if (curTime > timeSinceOrientationLastSent + orientationSendTime)
  {
    if (sendLevel >= 1)
    {

      sendEvent(IMUEvent(IMUEvent::OrientationUpdate, orientation, 3));
      if (sendLevel >= 2)
      {
        sendEvent(IMUEvent(IMUEvent::AccelUpdate, accel, 3));
        sendEvent(IMUEvent(IMUEvent::LinearAccelUpdate, linearAccel, 3));
        sendEvent(IMUEvent(IMUEvent::GyroUpdate, gyro, 3));
        //sendEvent(IMUEvent(IMUEvent::Gravity, gravity, 3));
      }
    }

    timeSinceOrientationLastSent = curTime;
  }
#endif
}

void IMUManager::computeThrow()
{

  float maxAccelYZ = max(abs(accel[1]), abs(accel[2]));
  float maxAccel = max(maxAccelYZ, abs(accel[0]));
  float maxLinearAccel = max(max(abs(linearAccel[0]), abs(linearAccel[1])), abs(linearAccel[2]));
  float accLinearDiff = abs(maxAccelYZ - maxLinearAccel);

  bool curIsFlat = throwState == 1;
  float flatThresh = curIsFlat ? flatThresholds[1] : flatThresholds[0];
  bool isFlatting = maxAccel < flatThresh;

  int newState = 0;
  if (isFlatting)
  {
    newState = 1; //flat
  }
  else
  {
    bool curIsThrowing = throwState > 1;
    float throwThresh = curIsThrowing ? accelThresholds[1] : accelThresholds[0];

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
        newState = 3; //double
    }
  }

  //NDBG(String(throwState));

  if (newState != throwState)
  {
    throwState = newState;
    sendEvent(IMUEvent(IMUEvent::ThrowState));
  }
}

void IMUManager::setEnabled(bool value)
{
  if (isEnabled == value)
    return;
  isEnabled = value;
}

bool IMUManager::handleCommand(String command, var *data, int numData)
{
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
    uint8_t system, gyro, accel, mag;
    system = gyro = accel = mag = 0;
    bno.getCalibration(&system, &gyro, &accel, &mag);

    calibration[0] = (float)system;
    calibration[1] = (float)gyro;
    calibration[2] = (float)accel;
    calibration[3] = (float)mag;

    sendEvent(IMUEvent(IMUEvent::CalibrationStatus, calibration, 4));
    return true;
  }
  else if (checkCommand(command, "accelThresholds", numData, 2))
  {
    accelThresholds[0] = data[0].floatValue();
    accelThresholds[1] = data[1].floatValue();
    prefs.begin(name.c_str());
    prefs.putFloat("throwMin", accelThresholds[0]);
    prefs.putFloat("throwMax", accelThresholds[1]);
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

  return false;
}
