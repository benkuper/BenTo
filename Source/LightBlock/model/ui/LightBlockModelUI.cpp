/*
  ==============================================================================

    LightBlockModelUI.cpp
    Created: 10 Apr 2018 8:10:04pm
    Author:  Ben

  ==============================================================================
*/

#include "LightBlockModelUI.h"

LightBlockModelUI::LightBlockModelUI(LightBlockModel * model) :
	 BaseItemMinimalUI(model)
{
	int numBytes;
	
	const char * imgData = BinaryData::getNamedResource((StringUtil::toShortName(model->getTypeString()) + "_png").getCharPointer(), numBytes);
	modelImage = ImageCache::getFromMemory(imgData, numBytes);
	setSize(64,64);

	setRepaintsOnMouseActivity(true);
} 

LightBlockModelUI::~LightBlockModelUI()
{
}

void LightBlockModelUI::paint(Graphics & g)
{
	g.setColour(Colours::white.withAlpha(.1f));
	g.fillRoundedRectangle(getLocalBounds().toFloat(), 8);
	g.setColour(Colours::white.withAlpha(isMouseOver() ? .5f : 1.f));
	if (modelImage.getWidth() > 0) g.drawImage(modelImage, getLocalBounds().withSizeKeepingCentre(imageSize, imageSize).toFloat());

	if (modelImage.getWidth() == 0 || isMouseOver())
	{
		g.setColour(Colours::white);
		g.drawText(item->niceName, getLocalBounds(), Justification::centred);
	}
}

void LightBlockModelUI::resized()
{
}
