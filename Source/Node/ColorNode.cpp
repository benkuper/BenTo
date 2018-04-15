/*
  ==============================================================================

	ColorNode.cpp
	Created: 14 Apr 2018 1:00:32pm
	Author:  Ben

  ==============================================================================
*/

#include "ColorNode.h"
#include "ui/ColorNodeViewUI.h"

ColorNode::ColorNode(const String & name, var params) : 
	Node(name, params)
{
	mainColorsSlot = addColorSlot(false, "Colors");
}

ColorNode::~ColorNode()
{
}

Array<Colour> ColorNode::getColorsForProp(Prop * p)
{
	//to override by child classes

	Array<Colour> result;
	for (int i = 0; i < p->resolution->intValue(); i++)
	{
		result.add(Colours::black);
	}
	return result;
}

NodeViewUI * ColorNode::createUI()
{
	return new ColorNodeViewUI(this);
}
