/*
  ==============================================================================

    PropShape.cpp
    Created: 26 Jun 2020 11:43:29am
    Author:  bkupe

  ==============================================================================
*/

juce_ImplementSingleton(PropShapeLibrary)

PropShapeLibrary::PropShapeLibrary() :
    Manager("Prop Shapes")
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
    curve.reset(new PropShapeCurve());
    addChildControllableContainer(curve.get());

}

PropShape::
~PropShape()
{
}
