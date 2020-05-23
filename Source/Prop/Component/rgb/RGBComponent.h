/*
  ==============================================================================

    RGBComponent.h
    Created: 8 May 2020 3:09:02pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once

#include "../PropComponent.h"

class RGBPropComponent :
    public PropComponent
{
public:
    RGBPropComponent(Prop * prop, var params);
    ~RGBPropComponent();

    float updateRate;
    int resolution;
    bool invertDirection;

    FloatParameter* brightness;
};