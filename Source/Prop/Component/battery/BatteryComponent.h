/*
  ==============================================================================

    BatteryComponent.h
    Created: 8 May 2020 3:08:03pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once

#include "../PropComponent.h"

class BatteryPropComponent :
    public PropComponent
{
public:
    BatteryPropComponent(Prop* prop, var params);
    ~BatteryPropComponent();

    FloatParameter* level;
    FloatParameter* voltage;
    FloatParameter* rawValue;
    Trigger* resetBattery;
};