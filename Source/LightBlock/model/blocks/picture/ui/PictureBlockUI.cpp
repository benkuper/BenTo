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
	updateThumbnail();
}

PictureBlockUI::~PictureBlockUI()
{
}

void PictureBlockUI::updateThumbnail()
{
	if (pb->picture.getWidth() > 0) modelImage = Image(pb->picture);
	else LightBlockModelUI::updateThumbnail();
	repaint();
}

void PictureBlockUI::controllableFeedbackUpdateInternal(Controllable * c)
{
	LightBlockModelUI::controllableFeedbackUpdateInternal(c);
	if (c == pb->pictureFile)
	{
		updateThumbnail();
	}
}
