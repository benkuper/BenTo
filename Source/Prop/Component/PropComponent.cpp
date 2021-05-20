/*
  ==============================================================================

    PropComponent.cpp
    Created: 8 May 2020 3:06:15pm
    Author:  bkupe

  ==============================================================================
*/

PropComponent::PropComponent(Prop * prop, const String& name, bool canBeDisabled) :
    EnablingControllableContainer(name, canBeDisabled),
    prop(prop),
    feedbackEnabled(false)
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
    if(!p->isControllableFeedbackOnly && !excludeControlControllables.contains(p)) sendControl(p->shortName, p->value);
}

void PropComponent::onContainerTriggerTriggered(Trigger* t)
{
    EnablingControllableContainer::onContainerTriggerTriggered(t);
    if (!t->isControllableFeedbackOnly && !excludeControlControllables.contains(t)) sendControl(t->shortName);

}

void PropComponent::handleMessage(const String &msg, var value)
{
    Controllable * c = getControllableByName(msg);
    if (c == nullptr)
    {
        DBG("Nothing found for property " + msg);
        return;
    }

    if (c->type == Controllable::TRIGGER)
    {
        if (c->isControllableFeedbackOnly) ((Trigger*)c)->trigger();
    }
    else if(Parameter * p = (Parameter *)c)
    {
        if (p->type == p->ENUM) ((EnumParameter*)p)->setValueWithData(value[0]);
        else p->setValue(p->isComplex() ? value : value[0]);
    }
}
