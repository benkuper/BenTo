/*
  ==============================================================================

    ButtonsComponent.cpp
    Created: 8 May 2020 3:08:10pm
    Author:  bkupe

  ==============================================================================
*/

#include "ButtonsComponent.h"

ButtonsPropComponent::ButtonsPropComponent(Prop* prop, var params) :
    PropComponent(prop, "Buttons")
{
    int numButtons = params.getProperty("count", 1);
    
    for (int i = 0; i < numButtons; i++)
    {
        PropButton* pb = new PropButton("Button " + String(i+1));
        addChildControllableContainer(pb, true);
        buttons.add(pb);
    }
}

ButtonsPropComponent::~ButtonsPropComponent()
{

}

void ButtonsPropComponent::handleMessage(const String& msg, var value)
{
    PropButton * pb = buttons[value[0]];
    if (pb == nullptr) return;
    Controllable* c = pb->getControllableByName(msg);
    if (c == nullptr)
    {
        DBG("Not found");
        return;
    }
    if (c->type == Controllable::TRIGGER) ((Trigger*)c)->trigger();
    else ((Parameter*)c)->setValue(value[1]);
}



// BUTTON Clas

ButtonsPropComponent::PropButton::PropButton(const String& name) :
    ControllableContainer(name)
{
    pressed = addBoolParameter("Pressed", "Pressed", false);
    shortPress = addTrigger("Short Press", "");
    longPress = addTrigger("Long Press", "");
    veryLongPress = addTrigger("Very Long Press", "");
    multiPress = addIntParameter("MultiPress", "", 0, 0);

    for (auto& c : controllables) c->setControllableFeedbackOnly(true);
}

ButtonsPropComponent::PropButton::~PropButton() 
{

}
