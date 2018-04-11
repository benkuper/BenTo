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
	const char * imgData = BinaryData::getNamedResource((model->shortName + "_png").getCharPointer(), numBytes);
	modelImage = ImageCache::getFromMemory(imgData, numBytes);
	setSize(64,64);
}

LightBlockModelUI::~LightBlockModelUI()
{
}

void LightBlockModelUI::paint(Graphics & g)
{
	g.setColour(Colours::white.withAlpha(.1f));
	g.fillRoundedRectangle(getLocalBounds().toFloat(), 8);
	g.drawImage(modelImage, getLocalBounds().toFloat());
}

void LightBlockModelUI::resized()
{
}
