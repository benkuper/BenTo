/*
  ==============================================================================

    TouchComponent.cpp
    Created: 19 Apr 2021 3:11:54pm
    Author:  bkupe

  ==============================================================================
*/

#include "TouchComponent.h"

TouchPropComponent::TouchPropComponent(Prop* prop, var params) :
    PropComponent(prop, "Touch")
{
    feedbackEnabled = true;
    int numTouch = params.getProperty("count", 1);

    for (int i = 0; i < numTouch; i++)
    {
        PropButton* pb = new PropButton("Button " + String(i + 1));
        addChildControllableContainer(pb, true);
        buttons.add(pb);
    }
}

TouchPropComponent::~TouchPropComponent()
{

}

void TouchPropComponent::handleMessage(const String& msg, var value)
{
    PropButton* pb = buttons[value[0]];
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

TouchPropComponent::PropButton::PropButton(const String& name) :
    ControllableContainer(name)
{
    pressed = addBoolParameter("Pressed", "Pressed", false);
    shortPress = addTrigger("Short Press", "");
    longPress = addTrigger("Long Press", "");
    veryLongPress = addTrigger("Very Long Press", "");
    multiPress = addIntParameter("MultiPress", "", 0, 0);

    for (auto& c : controllables) c->setControllableFeedbackOnly(true);
}

TouchPropComponent::PropButton::~PropButton()
{

}
