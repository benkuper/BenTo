/*
  ==============================================================================

    PropShapeCurveEasing.h
    Created: 26 Jun 2020 1:37:51pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"

class PropShapeCurveEasing
{
public:
    PropShapeCurveEasing(Easing2D * easing2D);
    virtual ~PropShapeCurveEasing();

    Easing2D* easing2D;
    IntParameter * numPixels;
};

class PSLinearEasing :
    public LinearEasing2D,
    public PropShapeCurveEasing
{
public:
    PSLinearEasing() : PropShapeCurveEasing(this) {}
    virtual ~PSLinearEasing() {}

    Easing2DUI* createUI() override;
};

class PSBezierEasing :
    public CubicEasing2D,
    public PropShapeCurveEasing
{
public:
    PSBezierEasing() : PropShapeCurveEasing(this) {}
    virtual ~PSBezierEasing() {}

    Easing2DUI* createUI() override;
};
