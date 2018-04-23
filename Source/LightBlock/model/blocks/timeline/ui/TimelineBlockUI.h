/*
  ==============================================================================

    TimelineBlockUI.h
    Created: 23 Apr 2018 11:36:00am
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "../../../ui/LightBlockModelUI.h"
#include "../TimelineBlock.h"

class TimelineBlockUI :
	public LightBlockModelUI
{
public:
	TimelineBlockUI(TimelineBlock * block);
	~TimelineBlockUI();

	TimelineBlock * tb;

	void editBlock() override;
};