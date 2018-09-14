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

Array<Colour> SolidColorNode::getColors(int id, int resolution, double time, var params)
{
	Array<Colour> result;
	result.resize(resolution);
	result.fill(color->getColor());
	return result;
}
