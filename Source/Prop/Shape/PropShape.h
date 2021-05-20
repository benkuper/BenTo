/*
  ==============================================================================

    PropShape.h
    Created: 26 Jun 2020 11:43:29am
    Author:  bkupe

  ==============================================================================
*/

#pragma once

class PropShapeCurve;

class PropShape :
    public BaseItem
{
public:
    PropShape();
    ~PropShape();

    std::unique_ptr<PropShapeCurve> curve;
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