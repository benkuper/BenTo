/*
  ==============================================================================

    RGBComponent.cpp
    Created: 8 May 2020 3:09:02pm
    Author:  bkupe

  ==============================================================================
*/

#include "RGBComponent.h"
#include "../../Prop.h"

RGBPropComponent::RGBPropComponent(Prop* prop, var params) :
    PropComponent(prop, "RGB"),
    updateRate(params.getProperty("updateRate", 50)),
    resolution(params.getProperty("resolution",32)),
    invertDirection(params.getProperty("invertDirection", false))
{
    brightness = addFloatParameter("Brightness", "Brightness", params.getProperty("defaultBrightness",.5f), 0, 1);
}

RGBPropComponent::~RGBPropComponent()
{
}

void RGBPropComponent::handePropConnected()
{
    PropComponent::handePropConnected();
    sendControl("enabled", prop->enabled->boolValue());
}
