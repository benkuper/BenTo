#include "../../common/Common.h"

#include <Wire.h>
#include "Adafruit_BNO055/Adafruit_BNO055.h"

#define TRAIL_MAX 20

class IMUEvent
{
public:
    enum Type { OrientationUpdate, AccelUpdate, GyroUpdate, LinearAccelUpdate, Gravity, ThrowState, CalibrationStatus, TYPES_MAX };
    static const String eventNames[TYPES_MAX];

    IMUEvent(Type t, float * data = nullptr, int numData = 0) : type(t), data(data), numData(numData) {}
    Type type;
    float * data;
    int intData;
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
    int sendLevel; //0 = throws / 1 = +orientation / 2 = + other stuff
    
    long orientationSendTime;
    long timeSinceOrientationLastSent;

    float calibration[4];
    
    float orientation[3];
    float accel[3];
    float gyro[3];
    float linearAccel[3];
    float gravity[3];
    
    int throwState; //0 = none, 1 = flat, 2 = single, 3 = double+, 4 = flat-front, 5 = loftie
    
    //IMU Compute
    float flatThresholds[2];
    float accelThresholds[2];
    float diffThreshold;
    float semiFlatThreshold;
    float loftieThreshold;
    float singleThreshold;

    
    void init();
    void update();

    void computeThrow();

    void setEnabled(bool value);


    bool handleCommand(String command, var * data, int numData) override;

    #ifdef USE_PREFERENCES
    Preferences prefs;
    #endif
};
