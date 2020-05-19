/*
  ==============================================================================

    BrightnessContrastBlockFilter.h
    Created: 18 May 2020 3:27:52pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once

#include "../../LightBlockFilter.h"

class BrightnessContrastBlockFilter :
    public LightBlockFilter
{
public:
    BrightnessContrastBlockFilter(var params = var());
    ~BrightnessContrastBlockFilter();

    FloatParameter* brightness;
    FloatParameter* contrast;

    virtual void getColorsInternal(Array<Colour>* result, Prop* p, double time, int id, int resolution, var params) override;

    String getTypeString() const override { return "Brightness Contrast"; }
};