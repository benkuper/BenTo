/*
  ==============================================================================

	PositionRemapNode.cpp
	Created: 13 Apr 2018 11:30:20pm
	Author:  Ben

  ==============================================================================
*/

#include "PositionRemapNode.h"


PositionRemapNode::PositionRemapNode(var params) :
	ColorNode(getTypeString(), params)
{
	position = addFloatParameter("Position", "Center position of the color input", .25f, 0, 1);
	size = addFloatParameter("Size", "Size of the color input", .5f, 0, 1);

	inColors = addColorSlot(true, "Colors");
	positionSlot = addParameterSlot(true, position);
	sizeSlot = addParameterSlot(true, size);
}

PositionRemapNode::~PositionRemapNode()
{
}

Array<Colour> PositionRemapNode::getColorsForProp(Prop * p)
{
	int numLeds = size->floatValue()*p->resolution->intValue();
	int firstLed = (position->floatValue() - size->floatValue() / 2) *p->resolution->intValue();
	fakeProp.resolution->setValue(numLeds);
	fakeProp.id->setValue(p->id->value);

	Array<Colour> result = ColorNode::getColorsForProp(p);
	Array<Colour> c = inColors->getColorsForProp(&fakeProp);

	for (int i = 0; i < numLeds; i++)
	{
		int index = firstLed + i;
		if (index < result.size()) result.set(index, c[i]);
	}
	return result;
}
