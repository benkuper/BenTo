/*
  ==============================================================================

    PropShapeView.cpp
    Created: 26 Jun 2020 12:04:08pm
    Author:  bkupe

  ==============================================================================
*/

PropShapeView::PropShapeView(PropShape * shape) :
    Curve2DUI(shape->curve.get()),
    shape(shape)
{
    addExistingItems();
}

PropShapeView::~PropShapeView()
{

}
