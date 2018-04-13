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
	const int imageSize = 40;

	void paint(Graphics &g) override;
	void resized() override;

	void mouseDown(const MouseEvent &e) override;
};