/*
  ==============================================================================

    LightBlockModelManagerUI.h
    Created: 10 Apr 2018 7:56:53pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "../UserLightBlockModelManager.h"
#include "LightBlockModelUI.h"

class LightBlockModelManagerUI :
	public BaseManagerUI<UserLightBlockModelManager, LightBlockModel, LightBlockModelUI>
{
public:
	LightBlockModelManagerUI(const String &name, UserLightBlockModelManager * manager);
	~LightBlockModelManagerUI();

	int thumbSize = 64;
	const int gap = 2;
	const int headerHeight = 16;
	const int headerGap = 4;

	void paint(Graphics &g) override;
	void resized() override;

	LightBlockModelUI * createUIForItem(LightBlockModel * i) override;

};