/*
  ==============================================================================

    PropShapeView.cpp
    Created: 26 Jun 2020 12:04:08pm
    Author:  bkupe

  ==============================================================================
*/

#include "PropShapeView.h"

PropShapeView::PropShapeView(PropShape * shape) :
    Curve2DUI(&shape->curve),
    shape(shape)
{
    addExistingItems();
}

PropShapeView::~PropShapeView()
{

}
