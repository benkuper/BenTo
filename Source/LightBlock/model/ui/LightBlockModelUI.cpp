/*
  ==============================================================================

    LightBlockModelUI.cpp
    Created: 10 Apr 2018 8:10:04pm
    Author:  Ben

  ==============================================================================
*/

#include "LightBlockModelUI.h"
#include "Prop/PropManager.h"

LightBlockModelUI::LightBlockModelUI(LightBlockModel * model) :
	 BaseItemMinimalUI(model)
{
	int numBytes;
	
	const char * imgData = BinaryData::getNamedResource((StringUtil::toShortName(model->getTypeString()) + "_png").getCharPointer(), numBytes);
	modelImage = ImageCache::getFromMemory(imgData, numBytes);
	setSize(64,64);

	setRepaintsOnMouseActivity(true);
	removeMouseListener(this);
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

void LightBlockModelUI::mouseDown(const MouseEvent & e)
{
	BaseItemMinimalUI::mouseDown(e);

	if (e.mods.isRightButtonDown() && e.mods.isPopupMenu())
	{
		PopupMenu menu;
		PopupMenu assignMenu;

		int index = 1;
		for (auto & p : PropManager::getInstance()->items)
		{
			assignMenu.addItem(index, p->niceName);
			index++;
		}
		
		menu.addItem(-1, "Assign to all");
		menu.addSubMenu("Assign to...", assignMenu);
		
		int result = menu.show();
		if (result == 0) return;
		if (result == -1)
		{
			for (auto & p : PropManager::getInstance()->items)
			{
				p->activeProvider->setValueFromTarget(item);
			}
		}if (result > 0 && result <= PropManager::getInstance()->items.size())
		{
			Prop * p = PropManager::getInstance()->items[result - 1];
			p->activeProvider->setValueFromTarget(item);
		}
	}
}
