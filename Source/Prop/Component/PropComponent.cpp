/*
  ==============================================================================

    PropComponent.cpp
    Created: 8 May 2020 3:06:15pm
    Author:  bkupe

  ==============================================================================
*/

#include "PropComponent.h"
#include "../Prop.h"

PropComponent::PropComponent(Prop * prop, const String& name, bool canBeDisabled) :
    EnablingControllableContainer(name, canBeDisabled),
    prop(prop)
{
}

PropComponent::~PropComponent()
{
}

void PropComponent::sendControl(String control, var value)
{
    prop->sendControlToProp(shortName + "." + control, value);
}

void PropComponent::onContainerParameterChanged(Parameter* p)
{
    EnablingControllableContainer::onContainerParameterChanged(p);
    if(!p->isControllableFeedbackOnly) sendControl(p->shortName, p->value);
}

void PropComponent::handleMessage(const String &msg, var value)
{
    Controllable * c = getControllableByName(msg);
    if (c == nullptr)
    {
        DBG("Nothing found for property " + msg);
        return;
    }

    if (c->type == Controllable::TRIGGER) ((Trigger*)c)->trigger();
    else
    {
        var val = value.size() > 1?value:value[0];
        ((Parameter*)c)->setValue(value);
    }
}
