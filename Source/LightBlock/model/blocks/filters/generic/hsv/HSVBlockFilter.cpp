/*
  ==============================================================================

    HSVBlockFilter.cpp
    Created: 18 May 2020 3:27:36pm
    Author:  bkupe

  ==============================================================================
*/

#include "HSVBlockFilter.h"

HSVBlockFilter::HSVBlockFilter(var params) :
    LightBlockFilter(getTypeString(), params)
{

    hueOffset = paramsContainer->addFloatParameter("Hue Offset", "", 0);
    saturationOffset = paramsContainer->addFloatParameter("Saturation Offset", "", 0,-1,1);
    brightnessOffset = paramsContainer->addFloatParameter("Brightness Offset", "", 0, -1, 1);

    hueSpeed = paramsContainer->addFloatParameter("Hue Speed", "", 0);
    saturationSpeed = paramsContainer->addFloatParameter("Saturation Speed", "", 0);
    brightnessSpeed = paramsContainer->addFloatParameter("Brightness Speed", "", 0);
}

HSVBlockFilter::~HSVBlockFilter()
{
 
}

void HSVBlockFilter::getColorsInternal(Array<Colour>* result, Prop* p, double time, int id, int resolution, var params)
{
    float bHueOffset = getParamValue<float>(hueOffset, params);
    float bSaturationOffset = getParamValue<float>(saturationOffset, params);
    float bBrightnessOffset = getParamValue<float>(brightnessOffset, params);

    float bHueSpeed = getParamValue<float>(hueSpeed, params);
    float bSaturationSpeed = getParamValue<float>(saturationSpeed, params);
    float bBrightnessSpeed = getParamValue<float>(brightnessSpeed, params);

    for (int i = 0; i < resolution; i++)
    {
        Colour c = result->getUnchecked(i);
        float h = 0, s = 0, b = 0;
        c.getHSB(h, s, b);
        h += bHueOffset + (bHueSpeed * time);
        s += jlimit<float>(0, 1, bSaturationOffset + sinf(bSaturationSpeed * time));
        b += jlimit<float>(0, 1, bBrightnessOffset + sinf(bBrightnessSpeed * time));
        result->set(i, Colour::fromHSV(h, s, b, c.getFloatAlpha()));
    }
}
