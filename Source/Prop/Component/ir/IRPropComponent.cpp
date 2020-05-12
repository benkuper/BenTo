/*
  ==============================================================================

    IRPropComponent.cpp
    Created: 8 May 2020 4:23:53pm
    Author:  bkupe

  ==============================================================================
*/

#include "IRPropComponent.h"

IRPropComponent::IRPropComponent(Prop* prop, var params) :
    PropComponent(prop, "IR")
{
    brightness = addFloatParameter("Brightness", "Brightness", 0, 0, 1);
}

IRPropComponent::~IRPropComponent()
{
}
