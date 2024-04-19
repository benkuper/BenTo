/*
  ==============================================================================

    SharedTextureBlockUI.h
    Created: 23 Apr 2018 9:45:30pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

class SharedTextureBlockUI :
	public LightBlockModelUI
{
public:
	SharedTextureBlockUI(SharedTextureBlock * block);
	~SharedTextureBlockUI();

	std::unique_ptr<BoolToggleUI> liveUI;
	SharedTextureBlock * vb;

	void paint(Graphics &g) override;
	void resized() override;

	void editBlock() override;

};