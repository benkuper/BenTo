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
	ScopedPointer<TargetParameterUI> targetUI;
	ScopedPointer<FloatSliderUI> batteryUI;

	void paintOverChildren(Graphics &g) override;
	void mouseDown(const MouseEvent &e) override;
	void mouseUp(const MouseEvent &e) override;

	void resizedInternalHeader(Rectangle<int> &r) override;
	void resizedInternalContent(Rectangle<int> &r) override;

	void controllableFeedbackUpdateInternal(Controllable * c) override;
};