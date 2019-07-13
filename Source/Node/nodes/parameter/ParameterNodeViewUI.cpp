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
	bgColor = BLUE_COLOR.withSaturation(.3f).withBrightness(.3f);

	pn->parameter->addAsyncParameterListener(this);
	buildParameterUI();
}

ParameterNodeViewUI::~ParameterNodeViewUI()
{
	if (pn != nullptr && pn->parameter != nullptr) pn->parameter->removeAsyncParameterListener(this);
}

void ParameterNodeViewUI::buildParameterUI()
{
	if (pui != nullptr) removeChildComponent(pui.get());
	pui.reset(pn->parameter->createDefaultUI());
	addAndMakeVisible(pui.get());
	resized();
}

void ParameterNodeViewUI::resizedInternalContent(Rectangle<int>& r)
{
	r.setSize(200, 20);
	pui->setBounds(r);
}

void ParameterNodeViewUI::newMessage(const Parameter::ParameterEvent & e)
{
	if (e.type == Parameter::ParameterEvent::BOUNDS_CHANGED)
	{
		buildParameterUI();
	}
}
