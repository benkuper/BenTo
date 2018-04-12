/*
  ==============================================================================

    PropUI.cpp
    Created: 10 Apr 2018 7:00:05pm
    Author:  Ben

  ==============================================================================
*/

#include "PropUI.h"

PropUI::PropUI(Prop * p) :
	BaseItemUI(p),
	viz(p)
{
	idUI = p->id->createStepper();
	idUI->showLabel = true;

	addAndMakeVisible(idUI);

	addAndMakeVisible(&viz);
}

PropUI::~PropUI()
{
}

void PropUI::resizedInternalHeader(Rectangle<int>& r)
{
	r.removeFromRight(2);
	idUI->setBounds(r.removeFromRight(60));
}

void PropUI::resizedInternalContent(Rectangle<int> &r)
{
	viz.setBounds(r.reduced(2));
}
