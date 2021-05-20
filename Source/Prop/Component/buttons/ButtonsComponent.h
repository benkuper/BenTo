/*
  ==============================================================================

    ButtonsComponent.h
    Created: 8 May 2020 3:08:10pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once

class ButtonsPropComponent :
    public PropComponent
{
public:
    ButtonsPropComponent(Prop * prop, var params);
    ~ButtonsPropComponent();

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