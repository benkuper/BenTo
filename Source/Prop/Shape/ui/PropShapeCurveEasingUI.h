/*
  ==============================================================================

    PropShapeCurveEasingUI.h
    Created: 26 Jun 2020 2:21:33pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once

#include "../PropShapeCurveEasing.h"

class PropShapeEasingUIDrawer
{
public:
    PropShapeEasingUIDrawer(Easing2DUI* eui, PropShapeCurveEasing * ce);
    virtual ~PropShapeEasingUIDrawer() {}

    PropShapeCurveEasing* ce;
    Easing2DUI* eui;
    void paintPixels(Graphics &g);
};

class PropShapeBezierEasingUI :
    public CubicEasing2DUI,
    public PropShapeEasingUIDrawer
{
public:
    PropShapeBezierEasingUI(PSBezierEasing* e) :
        CubicEasing2DUI(e),
        PropShapeEasingUIDrawer(this, e) {}

    void paintOverChildren(Graphics& g) override { paintPixels(g); }

};

class PropShapeLinearEasingUI :
    public LinearEasing2DUI,
    public PropShapeEasingUIDrawer
{
public:
    PropShapeLinearEasingUI(PSLinearEasing* e) :
        LinearEasing2DUI(e),
        PropShapeEasingUIDrawer(this, e) {}

    void paintOverChildren(Graphics& g) override { paintPixels(g); }

};