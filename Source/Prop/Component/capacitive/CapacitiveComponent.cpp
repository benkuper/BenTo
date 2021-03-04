/*
  ==============================================================================

    ButtonsComponent.cpp
    Created: 8 May 2020 3:08:10pm
    Author:  bkupe

  ==============================================================================
*/

#include "CapacitiveComponent.h"

CapacitivePropComponent::CapacitivePropComponent(Prop* prop, var params) :
    PropComponent(prop, "Capacitive")
{
    feedbackEnabled = true;
    int numButtons = params.getProperty("count", 1);

    for (int i = 0; i < numButtons; i++)
    {
        CapZone* pb = new CapZone("Cap " + String(i + 1));
        addChildControllableContainer(pb, true);
        zones.add(pb);
    }

    calibrate = addTrigger("Calibrate", "Calibrate the zones");
    diffRange = addFloatParameter("Sense Range", "Range of value differential", 50, 1);
    offset = addFloatParameter("Sense Offset", "Range of value differential", 0);
    smooth = addFloatParameter("Smooth", "Range of value differential", .8f, 0, 1);
}

CapacitivePropComponent::~CapacitivePropComponent()
{
}

void CapacitivePropComponent::onContainerTriggerTriggered(Trigger* t)
{
    PropComponent::onContainerTriggerTriggered(t);

    if (t == calibrate)
    {
        for (auto& z : zones)
        {
            z->refValue = z->rawValue->floatValue();
        }
    }
}

void CapacitivePropComponent::handleMessage(const String& msg, var value)
{
    if (msg.endsWith("touch"))
    {
        CapZone* pb = zones[value[0]];
        if (pb == nullptr) return;
        pb->pressed->setValue(value[1]);
    }
    else if(msg.endsWith("values"))
    {
        for (int i = 0; i < zones.size(); i++)
        {
            zones[i]->rawValue->setValue(value[i]);

            float offsetVal = zones[i]->refValue + offset->floatValue();
            float mappedValue = jmap<float>(value[i], offsetVal, offsetVal - diffRange->floatValue(), 0, 1);
            zones[i]->value->setValue(zones[i]->value->getLerpValueTo(mappedValue, 1- smooth->floatValue()));
        }
    }
   
}

CapacitivePropComponent::CapZone::CapZone(const String& name) :
    ControllableContainer(name),
    refValue(0)
{
    pressed = addBoolParameter("Touched", "Is touch detected on this zone ?", false);
    pressed->setControllableFeedbackOnly(true);

    rawValue = addFloatParameter("Raw Value", "Capacitance that is detected on this zone", 0, 100, 400);
    rawValue->setControllableFeedbackOnly(true);

    value = addFloatParameter("Value", "Calibrated and mapped from raw Value", 0, 0, 1);
    value->setControllableFeedbackOnly(true);
}

CapacitivePropComponent::CapZone::~CapZone()
{
}
