/*
  ==============================================================================

    BrightnessContrastBlockFilter.cpp
    Created: 18 May 2020 3:27:52pm
    Author:  bkupe

  ==============================================================================
*/

#include "BrightnessContrastBlockFilter.h"

BrightnessContrastBlockFilter::BrightnessContrastBlockFilter(var params) :
    LightBlockFilter(getTypeString(), params)
{
    brightness = paramsContainer->addFloatParameter("Brightness", "Brightness adjustment", 0, -1, 1);
    contrast = paramsContainer->addFloatParameter("Contrast", "Contrast adjustment", 0, -1, 1);
}

BrightnessContrastBlockFilter::~BrightnessContrastBlockFilter()
{
}

void BrightnessContrastBlockFilter::getColorsInternal(Array<Colour>* result, Prop* p, double time, int id, int resolution, var params)
{
    float bBrightness = getParamValue<float>(brightness, params);
    float bContrast = getParamValue<float>(contrast, params);


    for (int i = 0; i < resolution; i++)
    {
        Colour c = result->getUnchecked(i);
        float b = c.getBrightness();
        b = jlimit<float>(0, 1, b + bBrightness);
        b = jmap<float>(bContrast, b, b >= .5f ? 1 : 0);
        result->set(i, c.withBrightness(b));
    }
}
