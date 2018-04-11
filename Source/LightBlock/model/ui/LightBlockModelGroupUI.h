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

	void paint(Graphics &g) override;
	void resized() override;
};