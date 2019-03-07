/*
  ==============================================================================

	PositionRemapNode.cpp
	Created: 13 Apr 2018 11:30:20pm
	Author:  Ben

  ==============================================================================
*/

#include "PositionRemapNode.h"
#include "Prop/Prop.h"

PositionRemapNode::PositionRemapNode(var params) :
	ColorNode(getTypeString(), params)
{
	position = addFloatParameter("Position", "Center position of the color input", .25f, 0, 1);
	size = addFloatParameter("Size", "Size of the color input", .5f, 0, 1);
	mode = addEnumParameter("Mode", "Remap mode");
	mode->addOption("Resize", RESIZE)->addOption("Crop", CROP);

	inColors = addColorSlot(true, "Colors");
	positionSlot = addParameterSlot(true, position);
	sizeSlot = addParameterSlot(true, size);
}

PositionRemapNode::~PositionRemapNode()
{
}

Array<Colour> PositionRemapNode::getColorsInternal(Prop * p, double time, var params)
{
	int resolution = p->resolution->intValue();
	float bPosition = (float)getParameterValue(position, params);
	float bSize = (float)getParameterValue(size, params);
	int startLed = (bPosition - bSize / 2) * resolution;
	int endLed = (bPosition+bSize / 2) * resolution;

	Array<Colour> result = ColorNode::getColorsInternal(p, time, params);
	Array<Colour> c = inColors->getColors(p, time, params);

	RemapMode m = mode->getValueDataAsEnum<RemapMode>();

	for (int i = 0; i < resolution; i++)
	{

		if (i >= startLed && i < endLed)
		{
			int tLed = m == CROP ? i : jmap<float>(i, startLed, endLed, 0, resolution);
			result.set(i, c[tLed]);
		}
	}
	return result;
}
