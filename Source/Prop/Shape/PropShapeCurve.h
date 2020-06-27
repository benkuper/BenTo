/*
  ==============================================================================

    PropShapeCurve.h
    Created: 26 Jun 2020 11:59:50am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"

class PropShapeCurveKey :
    public Curve2DKey
{
public:
    PropShapeCurveKey();
    Easing2D* createEasingForType(int etype) override;
};

class PropShapeCurve :
    public Curve2D
{
public:
    PropShapeCurve();
    Curve2DKey* createItem() override;

    Array<Colour> getTestColors(int maxCount = -1);
};
