/*
  ==============================================================================

    NodeViewUI.h
    Created: 13 Apr 2018 11:25:28pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "../Node.h"

class NodeViewUI :
	public BaseItemUI<Node>
{
public:
	NodeViewUI(Node * node);
	~NodeViewUI();
};