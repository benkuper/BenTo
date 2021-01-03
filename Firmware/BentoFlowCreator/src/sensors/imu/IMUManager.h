#include "../../common/Common.h"

#include <Wire.h>
#include "Adafruit_BNO055/Adafruit_BNO055.h"

class IMUEvent
{
public:
    enum Type { OrientationUpdate, Shock, FreeFall, CalibrationStatus, TYPES_MAX };
    static const String eventNames[TYPES_MAX];

    IMUEvent(Type t) : type(t) {}
    Type type;
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

    float orientation[4];
    float calibration[4];
    
    void init();
    void update();

    void setEnabled(bool value);


    bool handleCommand(String command, var * data, int numData) override;
};
