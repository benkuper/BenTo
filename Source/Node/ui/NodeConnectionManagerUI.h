/*
  ==============================================================================

    NodeConnectionManagerUI.h
    Created: 14 Apr 2018 6:17:42pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "../NodeConnectionManager.h"
#include "NodeConnectionUI.h"

class NodeManagerUI;

class NodeConnectionManagerUI :
	public BaseManagerUI<NodeConnectionManager, NodeConnection, NodeConnectionUI>
{
public:
	NodeConnectionManagerUI(NodeConnectionManager * manager, NodeManagerUI * nodeManagerUI);
	~NodeConnectionManagerUI();

	NodeManagerUI * nmui;

	void resized() override;
	void paint(Graphics &g) override;
	
	void addItemUIInternal(NodeConnectionUI * nui) override;

};