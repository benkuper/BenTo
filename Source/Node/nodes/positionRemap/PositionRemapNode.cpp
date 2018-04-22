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

Array<Colour> PositionRemapNode::getColors(int id, int resolution, float time, var params)
{
	
	int firstLed = (position->floatValue() - size->floatValue() / 2) * resolution;

	Array<Colour> result = ColorNode::getColors(id, resolution, time, params);
	Array<Colour> c = inColors->getColors(id, resolution, time, params);

	for (int i = 0; i < resolution; i++)
	{
		int index = firstLed + i;
		if (index >= 0 && index < result.size()) result.set(index, c[i]);
	}
	return result;
}
