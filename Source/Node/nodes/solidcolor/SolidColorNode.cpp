/*
  ==============================================================================

    SolidColorNode.cpp
    Created: 13 Apr 2018 11:30:41pm
    Author:  Ben

  ==============================================================================
*/

#include "SolidColorNode.h"

SolidColorNode::SolidColorNode(var params) :
	ColorNode(getTypeString(), params)
{

	color = addColorParameter("Color", "Color to fill the prop", Colours::azure);
	addParameterSlot(true, color);
}

SolidColorNode::~SolidColorNode()
{
}

Array<Colour> SolidColorNode::getColorsForProp(Prop * p)
{
	Array<Colour> result;
	for (int i = 0; i < p->resolution->intValue(); i++)
	{
		result.add(color->getColor());
	}
	return result;
}
