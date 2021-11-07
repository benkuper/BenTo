#include "../../common/Common.h"

#ifdef CAPACITIVE_COUNT
#include "Adafruit_MPR121.h"
#endif
// You can have up to 4 on one i2c bus but one is enough for testing!

class CapacitiveEvent
{
public:
    enum Type { TouchUpdate, ValuesUpdate, TYPES_MAX };
    static const String eventNames[TYPES_MAX];

    CapacitiveEvent(Type t, float * data = nullptr, int numData = 0) : type(t), data(data), numData(numData) {}
    CapacitiveEvent(Type t, int index, bool balue) : type(t), index(index), value(value) {}
    Type type;
    float * data;
    int numData;

    int index;
    bool value;
};


class CapacitiveManager : public Component, public EventBroadcaster<CapacitiveEvent> {
public:
    CapacitiveManager();
    ~CapacitiveManager();

#ifdef CAPACITIVE_COUNT
    Adafruit_MPR121 cap;
    float values[CAPACITIVE_COUNT];
    bool touches[CAPACITIVE_COUNT];
#endif

    bool isConnected;
    bool isEnabled;
    int sendLevel; //0 = throws / 1 = +orientation / 2 = + other stuff
    
    long capSendTime;
    long timeSinceCapLastSent;

   
    uint16_t lastTouchStates;
    
    void init();
    void update();
    
    void setTouch(int index, bool value);
    void setEnabled(bool value);

    bool handleCommand(String command, var * data, int numData) override;

    #ifdef USE_PREFERENCES
    Preferences prefs;
    #endif
};
