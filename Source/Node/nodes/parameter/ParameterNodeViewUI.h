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
	public NodeViewUI
{
public:
	ParameterNodeViewUI(ParameterNode * pn);
	~ParameterNodeViewUI();

	ParameterNode * pn;
	ScopedPointer<ControllableUI> pui;

	void resizedInternalContent(Rectangle<int> &r) override;
};