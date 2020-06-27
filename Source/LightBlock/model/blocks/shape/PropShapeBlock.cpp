/*
  ==============================================================================

    PropShapeBlock.cpp
    Created: 26 Jun 2020 6:43:45pm
    Author:  bkupe

  ==============================================================================
*/

#include "PropShapeBlock.h"

ShapeEditorBlock::ShapeEditorBlock(var params) :
    LightBlockModel(getTypeString(), params)
{
    targetShape = paramsContainer->addTargetParameter("Shape", "Shape to use", PropShapeLibrary::getInstance());
    targetShape->maxDefaultSearchLevel = 0;
    targetShape->targetType = TargetParameter::CONTAINER;
}

ShapeEditorBlock::~ShapeEditorBlock()
{
}

void ShapeEditorBlock::getColorsInternal(Array<Colour>* result, Prop* p, double time, int id, int resolution, var params)
{
    if (targetShape->targetContainer.wasObjectDeleted()) return;

    if (PropShape* s = dynamic_cast<PropShape*>(targetShape->targetContainer.get()))
    {
       Array<Colour> colors = s->curve.getTestColors(resolution);
       result->swapWith(colors);
       result->resize(resolution);
    }
}