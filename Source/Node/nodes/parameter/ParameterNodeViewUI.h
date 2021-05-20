/*
  ==============================================================================

    ParameterNodeViewUI.h
    Created: 14 Apr 2018 5:07:05pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

class ParameterNodeViewUI :
	public NodeViewUI
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