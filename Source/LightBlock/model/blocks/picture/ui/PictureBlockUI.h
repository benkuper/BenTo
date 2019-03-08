/*
  ==============================================================================

    PictureBlockUI.h
    Created: 23 Apr 2018 11:49:28am
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "../../../ui/LightBlockModelUI.h"
#include "../PictureBlock.h"

class PictureBlockUI :
	public LightBlockModelUI
{
public:
	PictureBlockUI(PictureBlock * block);
	~PictureBlockUI();

	PictureBlock * pb;

	virtual void updateThumbnail() override;
	void controllableFeedbackUpdateInternal(Controllable * c) override;
};