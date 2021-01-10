#include "IMUManager.h"

const String IMUEvent::eventNames[IMUEvent::TYPES_MAX]{"orientation", "accel", "gyro", "linearAccel", "gravity", "shock", "flatfall", "calibration"};

IMUManager::IMUManager() : Component("imu"),
#ifdef HAS_IMU
                           bno(55),
#endif
                           isConnected(false),
                           isEnabled(false),
                           orientationSendTime(20), //50fps
                           timeSinceOrientationLastSent(0)

{
#ifdef HAS_IMU
  bno.setMode(Adafruit_BNO055::OPERATION_MODE_CONFIG);
  bno.setAxisRemap(Adafruit_BNO055::IMU_REMAP_CONFIG);
  bno.setAxisSign(Adafruit_BNO055::IMU_REMAP_SIGN);
  bno.setMode(Adafruit_BNO055::OPERATION_MODE_NDOF);
#endif
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

  imu::Vector<3> laccel = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
  linearAccel[0] = laccel.x();
  linearAccel[1] = laccel.y();
  linearAccel[2] = laccel.z();

  imu::Vector<3> acc = bno.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
  accel[0] = acc.x();
  accel[1] = acc.y();
  accel[2] = acc.z();

  imu::Vector<3> gyr = bno.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);
  gyro[0] = gyr.x();
  gyro[1] = gyr.y();
  gyro[2] = gyr.z();

  // imu::Vector<3> grav = bno.getVector(Adafruit_BNO055::VECTOR_GRAVITY);
  // gravity[0] = grav.x();
  // gravity[1] = grav.y();
  // gravity[2] = grav.z();

  long curTime = millis();
  if (curTime > timeSinceOrientationLastSent + orientationSendTime)
  {
    sendEvent(IMUEvent(IMUEvent::OrientationUpdate, orientation, 3));
    sendEvent(IMUEvent(IMUEvent::LinearAccelUpdate, linearAccel, 3));
    sendEvent(IMUEvent(IMUEvent::AccelUpdate, accel, 3));
    sendEvent(IMUEvent(IMUEvent::GyroUpdate, gyro, 3));
    // sendEvent(IMUEvent(IMUEvent::Gravity, gravity, 3));
    timeSinceOrientationLastSent = curTime;
  }
#endif
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

  return false;
}
