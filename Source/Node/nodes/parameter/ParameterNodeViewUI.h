/*
  ==============================================================================

    ParameterNodeViewUI.h
    Created: 14 Apr 2018 5:07:05pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "Node/ui/NodeViewUI.h"
#include "ParameterNode.h"

class ParameterNodeViewUI :
	public NodeViewUI,
	public Parameter::AsyncListener
{
public:
	ParameterNodeViewUI(ParameterNode * pn);
	~ParameterNodeViewUI();

	ParameterNode * pn;
	std::unique_ptr<ControllableUI> pui;

	void buildParameterUI();
	void resizedInternalContent(Rectangle<int> &r) override;

	void newMessage(const Parameter::ParameterEvent &e) override;
};