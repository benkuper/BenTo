#include "../../common/Common.h"

#if HAS_IMU
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

class IMUEvent
{
public:
    enum Type { OrientationUpdate, Shock, FreeFall, TYPES_MAX };
    static const String eventNames[TYPES_MAX];

    IMUEvent(Type t) : type(t) {}
    Type type;
};


class IMUManager : public Component, public EventBroadcaster<IMUEvent> {
public:
    IMUManager();
    ~IMUManager();

    Adafruit_BNO055 bno;

    bool isConnected;
    bool isEnabled;
    
    long orientationSendTime;
    long timeSinceOrientationLastSent;

    float orientation[4];
    
    void init();
    void update();

    void setEnabled(bool value);


    bool handleCommand(String command, var * data, int numData) override;
};

#endif