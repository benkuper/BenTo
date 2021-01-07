#include "../../common/Common.h"

#include <Wire.h>
#include "Adafruit_BNO055/Adafruit_BNO055.h"

class IMUEvent
{
public:
    enum Type { OrientationUpdate, AccelUpdate, GyroUpdate, LinearAccelUpdate, Gravity, Shock, FlatFall, CalibrationStatus, TYPES_MAX };
    static const String eventNames[TYPES_MAX];

    IMUEvent(Type t, float * data = nullptr, int numData = 0) : type(t), data(data), numData(numData) {}
    Type type;
    float * data;
    int numData;
};


class IMUManager : public Component, public EventBroadcaster<IMUEvent> {
public:
    IMUManager();
    ~IMUManager();

#ifdef HAS_IMU
    Adafruit_BNO055 bno;
#endif

    bool isConnected;
    bool isEnabled;
    
    long orientationSendTime;
    long timeSinceOrientationLastSent;

    float calibration[4];
    
    float orientation[3];
    float accel[3];
    float gyro[3];
    float linearAccel[3];
    float gravity[3];
    
    void init();
    void update();

    void setEnabled(bool value);


    bool handleCommand(String command, var * data, int numData) override;
};
