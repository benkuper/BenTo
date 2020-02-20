#pragma once
#include "../../common/Common.h"

class ButtonEvent
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

    ButtonEvent(Type type, int value = 1) : type(type), value(value) {}

    Type type;
    int value;
};

class ButtonManager :
    public Component,
    public EventBroadcaster<ButtonEvent>
{
public:
    ButtonManager();
    ~ButtonManager(){}

    bool isPressed;
    bool isLongPressed;
    bool isVeryLongPressed;

    void init();
    void update();

private :
    const int longPressTime = 500;        //more than 500ms is long press
    const int veryLongPressTime = 1500;   //more than 1500ms is very long press
    const int shortPressTime = 500;       //less than 500ms is short press
    const int multiPressTime = 300;       //each new press shorter than 500ms after the previous one will increase the multiclick
    
    const int buttonPressDebounce = 5;    //denoising, needs five reads to validate a change
    int debounceCount;

    long timeAtPress;
    int multiPressCount;

};