#include "events/EventBroadcaster.h"

class Timer;
class TimerEvent
{
public:
    TimerEvent(Timer * timer) : timer(timer) {}
    Timer * timer;
};

class Timer : public EventBroadcaster<TimerEvent> {
public: 
    Timer(long time) : isRunning(false), timeAtStart(0), time(time)
    {}

    ~Timer() {}

    bool isRunning;
    long timeAtStart;
    long time;

    void start()
    {
        timeAtStart = millis();
        isRunning = true;
    }

    void stop()
    {
        isRunning = false;
    }

    void update()
    {
        if(!isRunning) return;
        if(millis() > timeAtStart + time)
        {
            isRunning = false;
            sendEvent(TimerEvent(this));
        }
    }
};