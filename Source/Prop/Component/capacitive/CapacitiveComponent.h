/*
  ==============================================================================

    CapacitiveComponent.h
    Created: 4 Mar 2021 12:28:00pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once

#include "../PropComponent.h"

class CapacitivePropComponent :
    public PropComponent
{
public:
    CapacitivePropComponent(Prop * prop, var params);
    ~CapacitivePropComponent();

    Trigger* calibrate;
    FloatParameter* diffRange;
    FloatParameter* offset;
    FloatParameter* smooth;

    void onContainerTriggerTriggered(Trigger* t) override;

    class CapZone :
        public ControllableContainer
    {
    public:
        CapZone(const String& name);
        ~CapZone();

        float refValue;

        BoolParameter* pressed;
        FloatParameter* rawValue;
        FloatParameter* value;
    };

    Array<CapZone *> zones;

    void handleMessage(const String& msg, var value = var()) override;
};