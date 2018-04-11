/*
  ==============================================================================

    LightBlockModelUI.h
    Created: 10 Apr 2018 8:10:04pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "../LightBlockModel.h"

class LightBlockModelUI :
	public BaseItemMinimalUI<LightBlockModel>
{
public:
	LightBlockModelUI(LightBlockModel * model);
	~LightBlockModelUI();

	Image modelImage;

	void paint(Graphics &g) override;
	void resized() override;
};