#pragma once
#include "../../common/Common.h"

class TouchEvent
{
public:
    enum Type
    {
        Pressed,
        ShortPress,
        LongPress,
        VeryLongPress,
        MultiPress,
        TYPES_MAX
    };

    static const String eventNames[TYPES_MAX];

    TouchEvent(Type type, int id, int value = 1) : type(type), id(id), value(value) {}

    Type type;
    int id;
    int value;
};

class TouchManager :
    public Component,
    public EventBroadcaster<TouchEvent>
{
public:
    TouchManager();
    ~TouchManager(){}

    int touchThreshold;

#ifdef TOUCH_COUNT
    bool isPressed[TOUCH_COUNT];
    bool isLongPressed[TOUCH_COUNT];
    bool isVeryLongPressed[TOUCH_COUNT];
#endif

    void init();
    void update();

    void setTouchThreshold(int value);

    static void touchCallback();

    bool handleCommand(String command, var *data, int numData) override;

private :
    const int longPressTime = 500;        //more than 500ms is long press
    const int veryLongPressTime = 1500;   //more than 1500ms is very long press
    const int shortPressTime = 500;       //less than 500ms is short press
    const int multiPressTime = 300;       //each new press shorter than 500ms after the previous one will increase the multiclick
    
    const int touchPressDebounce = 5;    //denoising, needs five reads to validate a change

#ifdef TOUCH_COUNT
    int debounceCount[TOUCH_COUNT];
    
    long timeAtPress[TOUCH_COUNT];
    int multiPressCount[TOUCH_COUNT];
    #endif
};