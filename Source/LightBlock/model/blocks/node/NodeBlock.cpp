/*
  ==============================================================================

	NodeBlock.cpp
	Created: 10 Apr 2018 6:59:08pm
	Author:  Ben

  ==============================================================================
*/

#include "NodeBlock.h"

NodeBlock::NodeBlock(var params) :
	LightBlockModel(getTypeString(), params)
{
	addChildControllableContainer(&manager);
}

NodeBlock::~NodeBlock()
{
}
