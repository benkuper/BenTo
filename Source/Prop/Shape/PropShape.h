/*
  ==============================================================================

    PropShape.h
    Created: 26 Jun 2020 11:43:29am
    Author:  bkupe

  ==============================================================================
*/

#pragma once

#include "PropShapeCurve.h"


class PropShape :
    public BaseItem
{
public:
    PropShape();
    ~PropShape();

    PropShapeCurve curve;
};


class PropShapeLibrary :
    public BaseManager<PropShape>
{
public:
    juce_DeclareSingleton(PropShapeLibrary, true);
    PropShapeLibrary();
    ~PropShapeLibrary();

    void loadShapes();

};