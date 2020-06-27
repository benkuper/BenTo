/*
  ==============================================================================

    PropShape.cpp
    Created: 26 Jun 2020 11:43:29am
    Author:  bkupe

  ==============================================================================
*/

#include "PropShape.h"

juce_ImplementSingleton(PropShapeLibrary)

PropShapeLibrary::PropShapeLibrary() :
    BaseManager("Prop Shapes")
{
}

PropShapeLibrary::~PropShapeLibrary()
{
}


void PropShapeLibrary::loadShapes()
{
}

PropShape::PropShape() :
    BaseItem("New shape")
{
    addChildControllableContainer(&curve);

}

PropShape::
~PropShape()
{
}
