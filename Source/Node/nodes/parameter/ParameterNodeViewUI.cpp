/*
  ==============================================================================

    ParameterNodeViewUI.cpp
    Created: 14 Apr 2018 5:07:05pm
    Author:  Ben

  ==============================================================================
*/

#include "ParameterNodeViewUI.h"

ParameterNodeViewUI::ParameterNodeViewUI(ParameterNode * pn) :
	NodeViewUI(pn, Direction::NONE),
	pn(pn)
{
	pui = pn->parameter->createDefaultUI();
	addAndMakeVisible(pui);

	bgColor = BLUE_COLOR.withSaturation(.3f).withBrightness(.3f);
}

ParameterNodeViewUI::~ParameterNodeViewUI()
{
}

void ParameterNodeViewUI::resizedInternalContent(Rectangle<int>& r)
{
	r.setSize(200, 20);
	pui->setBounds(r);
}