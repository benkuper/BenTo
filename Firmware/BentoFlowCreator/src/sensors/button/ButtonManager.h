#pragma once
#include "../../common/Common.h"

class ButtonEvent
{
public:
    enum Type
    {
        Pressed,
        Released,
        ShortPress,
        LongPress,
        VeryLongPress,
        MultiPress,
        TYPES_MAX
    };

    const String typeNames[TYPES_MAX]{"Pressed","Released","ShortPress","LongPress","MultiPress" };

    ButtonEvent(Type type, int count = 0) : type(type), count(count) {}

    Type type;
    int count;
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