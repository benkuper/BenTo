/*
  ==============================================================================

    LiveFeedBlockUI.h
    Created: 23 Apr 2018 9:45:30pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

class LiveFeedBlockUI :
	public LightBlockModelUI
{
public:
	LiveFeedBlockUI(LiveFeedBlock * block);
	~LiveFeedBlockUI();

	std::unique_ptr<BoolToggleUI> liveUI;
	LiveFeedBlock * vb;

	void paint(Graphics &g) override;
	void resized() override;

	void editBlock() override;

};