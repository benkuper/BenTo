/*
  ==============================================================================

    PropShapeCurveEasing.cpp
    Created: 26 Jun 2020 1:37:51pm
    Author:  bkupe

  ==============================================================================
*/

PropShapeCurveEasing::PropShapeCurveEasing(Easing2D* easing2D) :
    easing2D(easing2D)
{
    easing2D->showInspectorOnSelect = true;
    numPixels = easing2D->addIntParameter("Num Pixels", "Number of pixels on this strip", 0, 0, 1000);
}

PropShapeCurveEasing::~PropShapeCurveEasing()
{
}

Easing2DUI* PSLinearEasing::createUI()
{
    return new PropShapeLinearEasingUI(this);
}

Easing2DUI* PSBezierEasing::createUI()
{
    return new PropShapeBezierEasingUI(this);
}
