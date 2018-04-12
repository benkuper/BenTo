/*
  ==============================================================================

    PropUI.h
    Created: 10 Apr 2018 7:00:05pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "../Prop.h"
#include "PropViz.h"

class PropUI :
	public BaseItemUI<Prop>
{
public:
	PropUI(Prop * p);
	~PropUI();

	PropViz viz;
	ScopedPointer<IntStepperUI> idUI;

	void resizedInternalHeader(Rectangle<int> &r) override;
	void resizedInternalContent(Rectangle<int> &r) override;
};