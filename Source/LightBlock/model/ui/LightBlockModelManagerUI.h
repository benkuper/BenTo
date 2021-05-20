/*
  ==============================================================================

    LightBlockModelManagerUI.h
    Created: 10 Apr 2018 7:56:53pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

class LightBlockModelManagerUI :
	public BaseManagerUI<UserLightBlockModelManager, LightBlockModel, LightBlockModelUI>
{
public:
	LightBlockModelManagerUI(const String &name, UserLightBlockModelManager * manager);
	~LightBlockModelManagerUI();

	int thumbSize = 48;
	const int gap = 2;
	const int headerHeight = 16;
	const int headerGap = 4;

	void paint(Graphics &g) override;
	void resized() override;

	void setThumbSize(int value);

	LightBlockModelUI * createUIForItem(LightBlockModel * i) override;

};