/*
  ==============================================================================

    LightBlockModelGroupUI.h
    Created: 10 Apr 2018 11:17:09pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "../LightBlockModel.h"
#include "LightBlockModelUI.h"

class LightBlockModelGroupUI :
	public Component
{
public:
	LightBlockModelGroupUI(ControllableContainer * group);
	~LightBlockModelGroupUI();

	ControllableContainer * group;
	OwnedArray<LightBlockModelUI> modelsUI;

	const int gap = 2;
	const int headerHeight = 16;
	const int headerGap = 4;
	int thumbSize = 48;

	void paint(Graphics &g) override;
	void resized() override;

	void setThumbSize(int value);

	bool keyPressed(const KeyPress & e) override;
};