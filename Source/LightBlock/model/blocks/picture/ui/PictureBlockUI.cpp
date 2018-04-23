/*
  ==============================================================================

	PictureBlockUI.cpp
	Created: 23 Apr 2018 11:49:28am
	Author:  Ben

  ==============================================================================
*/

#include "PictureBlockUI.h"

PictureBlockUI::PictureBlockUI(PictureBlock * block) :
	LightBlockModelUI(block),
	pb(block)
{
	modelImage = Image(pb->picture);
}

PictureBlockUI::~PictureBlockUI()
{
}

void PictureBlockUI::controllableFeedbackUpdateInternal(Controllable * c)
{
	LightBlockModelUI::controllableFeedbackUpdateInternal(c);
	if (c == pb->pictureFile)
	{
		modelImage = Image(pb->picture);
		repaint();
	}
}
