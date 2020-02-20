#include "IMUManager.h"
#if HAS_IMU

const String IMUEvent::eventNames[IMUEvent::TYPES_MAX] { "orientation","shock", "freefall" };

IMUManager::IMUManager() : Component("imu"),
                           bno(55),
                           isConnected(false),
                           isEnabled(true),
                           orientationSendTime(50),
                           timeSinceOrientationLastSent(0)

{
  bno.setMode(Adafruit_BNO055::OPERATION_MODE_CONFIG);
  bno.setAxisRemap(Adafruit_BNO055::REMAP_CONFIG_P0);
  bno.setAxisSign(Adafruit_BNO055::REMAP_SIGN_P0);
  bno.setMode(Adafruit_BNO055::OPERATION_MODE_NDOF);
}

IMUManager::~IMUManager()
{
}

void IMUManager::init()
{
  NDBG("Init");
  if(isConnected) return;
  Wire.begin(SDA_PIN, SCL_PIN);

  if (!bno.begin())
  {
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    isConnected = false;
  }

  bno.setMode(Adafruit_BNO055::OPERATION_MODE_CONFIG);
  bno.setAxisRemap(Adafruit_BNO055::REMAP_CONFIG_P0);
  bno.setAxisSign(Adafruit_BNO055::REMAP_SIGN_P0);
  bno.setMode(Adafruit_BNO055::OPERATION_MODE_NDOF);

  isConnected = true;
  bno.setExtCrystalUse(true);
  NDBG("Imu is connected.");
}

void IMUManager::update()
{
  if(!isEnabled || !isConnected) return;

  imu::Quaternion q = bno.getQuat();
  q.normalize();
  
  //float temp = q.x();  q.x() = -q.y();  q.y() = temp;
  //q.z() = -q.z();
  imu::Vector<3> euler = q.toEuler();
  orientation[0] = euler.x() * 180/PI;  //Yaw
  orientation[1] = euler.y() * 180/PI;  //Pitch
  orientation[2] = euler.z() * 180/PI;  //Roll
  
  long curTime = millis();
  if (curTime > timeSinceOrientationLastSent + orientationSendTime)
  {
    sendEvent(IMUEvent(IMUEvent::OrientationUpdate));
    timeSinceOrientationLastSent = curTime;
  }
}

void IMUManager::setEnabled(bool value)
{
  if(isEnabled == value) return;
  isEnabled = value;
}

bool IMUManager::handleCommand(String command, var *data, int numData)
{
  if(checkCommand(command, "enabled", numData, 1))
  {
    setEnabled(data[0].intValue());
  }else if(checkCommand(command, "updateRate", numData, 1))
  {
    orientationSendTime = 1000/data[0].intValue();
  }

  return false;
}

#endif