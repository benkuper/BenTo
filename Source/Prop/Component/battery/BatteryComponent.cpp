/*
  ==============================================================================

    BatteryComponent.cpp
    Created: 8 May 2020 3:08:03pm
    Author:  bkupe

  ==============================================================================
*/

#include "BatteryComponent.h"

BatteryPropComponent::BatteryPropComponent(Prop* prop, var params) :
    PropComponent(prop, "Battery")
{
    level = addFloatParameter("Level", "Level of the battery", 1, 0, 1);
    level->setControllableFeedbackOnly(true);
}

BatteryPropComponent::~BatteryPropComponent()
{

}
