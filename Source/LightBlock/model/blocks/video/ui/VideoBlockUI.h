/*
  ==============================================================================

    VideoBlockUI.h
    Created: 23 Apr 2018 9:45:30pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

class VideoBlockUI :
	public LightBlockModelUI
{
public:
	VideoBlockUI(VideoBlock * block);
	~VideoBlockUI();

	std::unique_ptr<BoolToggleUI> liveUI;
	VideoBlock * vb;

	void paint(Graphics &g) override;
	void resized() override;

	void editBlock() override;

};