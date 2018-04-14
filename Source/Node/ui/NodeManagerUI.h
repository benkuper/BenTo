/*
  ==============================================================================

    NodeManagerUI.h
    Created: 14 Apr 2018 2:10:12am
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "../NodeManager.h"
#include "NodeViewUI.h"

class NodeManagerUI :
	public BaseManagerViewUI<NodeManager, Node, NodeViewUI>
{
public:
	NodeManagerUI(NodeManager * manager);
	~NodeManagerUI();

	
};