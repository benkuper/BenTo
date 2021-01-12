#include "IMUManager.h"

const String IMUEvent::eventNames[IMUEvent::TYPES_MAX]{"orientation", "accel", "gyro", "linearAccel", "gravity", "throwState", "calibration"};

IMUManager::IMUManager() : Component("imu"),
#ifdef HAS_IMU
                           bno(55),
#endif
                           isConnected(false),
                           isEnabled(false),
                           sendRawData(false),
                           orientationSendTime(20), //50fps
                           timeSinceOrientationLastSent(0),
                           throwState(0),
                           trailIndex(0),
                           inSpeed(false)
{
#ifdef HAS_IMU
  bno.setMode(Adafruit_BNO055::OPERATION_MODE_CONFIG);
  bno.setAxisRemap(Adafruit_BNO055::IMU_REMAP_CONFIG);
  bno.setAxisSign(Adafruit_BNO055::IMU_REMAP_SIGN);
  bno.setMode(Adafruit_BNO055::OPERATION_MODE_NDOF);
#endif

  trailCount = 5;
  throwThresholds[0] = .7f;
  throwThresholds[1] = 2;

  speedThresholds[0] = .2f;
  speedThresholds[1] = 1;

  flatThresholds[0] = .8f;
  flatThresholds[1] = 2;

  semiFlatThreshold = 2;
  loftieThreshold = 12;
  singleThreshold = 25;

  memset(trail, 0, TRAIL_MAX * 3 * sizeof(float));
  memset(smoothAccel, 0, 3 * sizeof(float));
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
  int trailCount = prefs.getFloat("trailCount", trailCount);
  throwThresholds[0] = prefs.getFloat("throwMin", throwThresholds[0]);
  throwThresholds[1] = prefs.getFloat("throwMax", throwThresholds[1]);
  speedThresholds[0] = prefs.getFloat("speedMin", speedThresholds[0]);
  speedThresholds[1] = prefs.getFloat("speedMax", speedThresholds[1]);
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

  if (sendRawData)
  {
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
  }

  computeThrow();

  long curTime = millis();
  if (curTime > timeSinceOrientationLastSent + orientationSendTime)
  {
    sendEvent(IMUEvent(IMUEvent::OrientationUpdate, orientation, 3));

    if (sendRawData)
    {
      sendEvent(IMUEvent(IMUEvent::AccelUpdate, accel, 3));
      sendEvent(IMUEvent(IMUEvent::LinearAccelUpdate, linearAccel, 3));
      sendEvent(IMUEvent(IMUEvent::GyroUpdate, gyro, 3));
      //sendEvent(IMUEvent(IMUEvent::Gravity, gravity, 3));
    }

    timeSinceOrientationLastSent = curTime;
  }
#endif
}

void IMUManager::computeThrow()
{
  trail[trailIndex][0] = accel[0];
  trail[trailIndex][1] = accel[1];
  trail[trailIndex][2] = accel[2];

  trailIndex = (trailIndex + 1) % trailCount;

  float sa[3]{0, 0, 0};
  for (int i = 0; i < trailCount; i++)
    for (int j = 0; j < 3; j++)
      sa[i] += trail[i][j] / trailCount;

  float aSpeed[3]{0, 0, 0};
  for (int i = 0; i < 3; i++)
    aSpeed[i] = sa[i] - smoothAccel[i];

  float maxYZSpeed = max(abs(aSpeed[1]), abs(aSpeed[1]));
  float maxSmoothed = max(max(abs(sa[0]), abs(sa[1])), abs(sa[2]));

  for (int i = 0; i < 3; i++)
    smoothAccel[i] = sa[i];

  bool curIsThrowing = throwState > 0;
  bool curIsFlat = throwState == 1;

  float throwThresh = curIsThrowing ? throwThresholds[1] : throwThresholds[0];
  float speedThresh = inSpeed ? speedThresholds[1] : speedThresholds[0];
  float flatThresh = curIsFlat ? flatThresholds[1] : flatThresholds[0];

  bool throwPotential = fabsf(sa[1]) < throwThresh;

  inSpeed = maxYZSpeed < speedThresh;

  bool isThrowing = throwPotential && inSpeed;
  bool isFlatting = maxSmoothed < flatThresh;

  int result = 0;
  if (isThrowing)
  {
    if (isFlatting)
      result = 1;
    else if (sa[0] < semiFlatThreshold)
      result = 4;
    else if (sa[0] < loftieThreshold)
      result = 5;
    else if (sa[0] < singleThreshold)
      result = 2;
    else
      result = 3;
  }

  if(result != throwState)
  {
    throwState = result;
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
  if (checkCommand(command, "sendRawData", numData, 1))
  {
    sendRawData = data[0].intValue();
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
  else if (checkCommand(command, "trailCount", numData, 1))
  {
    trailCount = min(data[0].intValue(), TRAIL_MAX);
    prefs.begin(name.c_str());
    prefs.putFloat("trailCount", trailCount);
    prefs.end();
  }
  else if (checkCommand(command, "throwThresholds", numData, 2))
  {
    throwThresholds[0] = data[0].floatValue();
    throwThresholds[1] = data[1].floatValue();
    prefs.begin(name.c_str());
    prefs.putFloat("throwMin", throwThresholds[0]);
    prefs.putFloat("throwMax", throwThresholds[1]);
    prefs.end();
  }
  else if (checkCommand(command, "speedThresholds", numData, 2))
  {
    speedThresholds[0] = data[0].floatValue();
    speedThresholds[1] = data[1].floatValue();
    prefs.begin(name.c_str());
    prefs.putFloat("speedMin", speedThresholds[0]);
    prefs.putFloat("speedMax", speedThresholds[1]);
    prefs.end();
  }
  else if (checkCommand(command, "flatThresholds", numData, 2))
  {
    flatThresholds[0] = data[0].floatValue();
    flatThresholds[1] = data[1].floatValue();
    prefs.begin(name.c_str());
    prefs.putFloat("flatMin", flatThresholds[0]);
    prefs.putFloat("flatMax", flatThresholds[1]);
    prefs.end();
  }
  else if (checkCommand(command, "semiFlatThreshold", numData, 1))
  {
    semiFlatThreshold = data[0].floatValue();
    prefs.begin(name.c_str());
    prefs.putFloat("semiFlat", semiFlatThreshold);
    prefs.end();
  }
  else if (checkCommand(command, "loftieThreshold", numData, 1))
  {
    loftieThreshold = data[0].floatValue();
    prefs.begin(name.c_str());
    prefs.putFloat("loftie", loftieThreshold);
    prefs.end();
  }
  else if (checkCommand(command, "singleThreshold", numData, 1))
  {
    singleThreshold = data[0].floatValue();
    prefs.begin(name.c_str());
    prefs.putFloat("single", singleThreshold);
    prefs.end();
  }

  return false;
}
