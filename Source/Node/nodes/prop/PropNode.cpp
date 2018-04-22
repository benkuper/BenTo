/*
  ==============================================================================

	PropNode.cpp
	Created: 13 Apr 2018 11:29:28pm
	Author:  Ben

  ==============================================================================
*/

#include "PropNode.h"

PropNode::PropNode(var params) :
	ColorNode(getTypeString(), params)
{
	userCanRemove = false;
	removeSlot(false, mainColorsSlot);
	inColors = addColorSlot(true, "Colors");
	isSavable = false;
}

PropNode::~PropNode()
{
}

Array<Colour> PropNode::getColors(int id, int resolution, float time, var params)
{
	Array<Colour> c = inColors->getColors(id, resolution, time, params);
	if (c.size() == 0) c = ColorNode::getColors(id, resolution, time, params);
	return c;
}