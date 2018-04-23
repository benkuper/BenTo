/*
  ==============================================================================

	NodeBlockUI.cpp
	Created: 23 Apr 2018 11:34:54am
	Author:  Ben

  ==============================================================================
*/

#include "NodeBlockUI.h"

NodeBlockUI::NodeBlockUI(NodeBlock * block) :
	LightBlockModelUI(block),
	nb(block)
{
}

NodeBlockUI::~NodeBlockUI()
{
}

void NodeBlockUI::editBlock()
{
	ShapeShifterManager::getInstance()->showContent("Node Editor");
}
