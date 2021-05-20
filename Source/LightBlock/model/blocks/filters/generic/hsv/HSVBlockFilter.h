/*
  ==============================================================================

    HSVBlockFilter.h
    Created: 18 May 2020 3:27:36pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once

class HSVBlockFilter :
    public LightBlockFilter
{
public:
    HSVBlockFilter(var params = var());
    ~HSVBlockFilter();

    FloatParameter* hueOffset;
    FloatParameter* saturationOffset;
    FloatParameter* brightnessOffset;

    FloatParameter* hueSpeed;
    FloatParameter* saturationSpeed;
    FloatParameter* brightnessSpeed;

    FloatParameter* idOffset;

    virtual void getColorsInternal(Array<Colour>* result, Prop* p, double time, int id, int resolution, var params) override;

    String getTypeString() const override { return "HSV"; }
};