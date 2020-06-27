/*
  ==============================================================================

    PropShapeCurve.cpp
    Created: 26 Jun 2020 11:59:50am
    Author:  bkupe

  ==============================================================================
*/

#include "PropShapeCurve.h"
#include "PropShapeCurveEasing.h"

PropShapeCurve::PropShapeCurve() :
    Curve2D("Curve")
{
}

Curve2DKey* PropShapeCurve::createItem()
{
    return new PropShapeCurveKey();
}

Array<Colour> PropShapeCurve::getTestColors(int maxCount)
{
    float hue = 0;
    Array<Colour> result;
    for (auto& section : items)
    {
        if (PropShapeCurveEasing* pe = dynamic_cast<PropShapeCurveEasing*>(section->easing.get()))
        {
            int numPixels = pe->numPixels->intValue();;
            for (int i = 0; i < numPixels; i++)
            {
                if (maxCount >= 0 && result.size() >= maxCount) return result;
                result.add(Colour::fromHSV(hue, 1, 1, 1));
            }
        }
        hue += .37f;
    }

    return result;
}



PropShapeCurveKey::PropShapeCurveKey() :
    Curve2DKey()
{
    showInspectorOnSelect = true;
    updateEasing(true);

}

Easing2D* PropShapeCurveKey::createEasingForType(int type)
{
    switch (type)
    {
    case Easing2D::LINEAR: return new PSLinearEasing();
    case Easing2D::BEZIER: return new PSBezierEasing();
    default:
        break;

    }
    return nullptr;
}