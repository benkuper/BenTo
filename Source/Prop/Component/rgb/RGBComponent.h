/*
  ==============================================================================

    RGBComponent.h
    Created: 8 May 2020 3:09:02pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once

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

    void handePropConnected() override;
};