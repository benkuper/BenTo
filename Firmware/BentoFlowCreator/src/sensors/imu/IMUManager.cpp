#include "IMUManager.h"

const String IMUEvent::eventNames[IMUEvent::TYPES_MAX] { "orientation","shock", "freefall" };

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
  bno.setAxisRemap(Adafruit_BNO055::REMAP_CONFIG_P0);
  bno.setAxisSign(Adafruit_BNO055::REMAP_SIGN_P0);
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
  if(isConnected) return;

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
  if(!isEnabled || !isConnected) return;

#ifdef HAS_IMU
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
#endif

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
    return true;
  }else if(checkCommand(command, "updateRate", numData, 1))
  {
    orientationSendTime = 1000/data[0].intValue();
    return true;
  }

  return false;
}
