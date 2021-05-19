/*
  ==============================================================================

    TouchComponent.h
    Created: 19 Apr 2021 3:11:54pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "../PropComponent.h"

class TouchPropComponent :
    public PropComponent
{
public:
    TouchPropComponent(Prop* prop, var params);
    ~TouchPropComponent();

    class PropButton :
        public ControllableContainer
    {
    public:
        PropButton(const String& name);
        ~PropButton();

        BoolParameter* pressed;
        Trigger* shortPress;
        Trigger* longPress;
        Trigger* veryLongPress;
        IntParameter* multiPress;
    };

    Array<PropButton*> buttons;


    void handleMessage(const String& msg, var value = var()) override;
};
