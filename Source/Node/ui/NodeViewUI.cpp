/*
  ==============================================================================

	NodeViewUI.cpp
	Created: 13 Apr 2018 11:25:28pm
	Author:  Ben

  ==============================================================================
*/

#include "NodeViewUI.h"

NodeViewUI::NodeViewUI(Node * node) :
	BaseItemUI(node, ResizeMode::ALL)
{
	setSize(200, 100);
}

NodeViewUI::~NodeViewUI()
{
}
