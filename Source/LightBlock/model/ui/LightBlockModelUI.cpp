/*
  ==============================================================================

	LightBlockModelUI.cpp
	Created: 10 Apr 2018 8:10:04pm
	Author:  Ben

  ==============================================================================
*/

#include "LightBlock/LightBlockIncludes.h"
#include "Prop/PropIncludes.h"
#include "Sequence/SequenceIncludes.h"

LightBlockModelUI::LightBlockModelUI(LightBlockModel* model) :
	BaseItemMinimalUI(model),
	sequenceBlock(dynamic_cast<BentoSequenceBlock*>(model))
{

	updateThumbnail();

	autoHideWhenDragging = false;

	setSize(64, 64);

	setRepaintsOnMouseActivity(true);
	model->addAsyncModelListener(this);
}

LightBlockModelUI::~LightBlockModelUI()
{
	if (item != nullptr) item->removeAsyncModelListener(this);
}

void LightBlockModelUI::paint(Graphics& g)
{
	Colour bc = item->itemColor->getColor();
	bgColor = item->isBeingEdited ? BLUE_COLOR.darker().withSaturation(.3f) : bc;

	Colour bgC = bgColor;
	if (sequenceBlock != nullptr && sequenceBlock->sequence->isPlaying->boolValue()) bgC = GREEN_COLOR.darker();
	g.setColour(bgC.brighter(isMouseOver() ? .2f : 0));
	g.fillRoundedRectangle(getLocalBounds().toFloat(), 2);
	g.setColour(Colours::white);// .withAlpha(isMouseOver() ? .2f : 1.f));
	if (modelImage.getWidth() > 0)
	{
		Rectangle<float> ir = RectanglePlacement().appliedTo(modelImage.getBounds().toFloat(), getLocalBounds().withTrimmedTop(14).reduced(4).toFloat());
		g.drawImage(modelImage, ir);
	}

	g.setColour(Colours::black.withAlpha(.5f));
	g.fillRoundedRectangle(getLocalBounds().removeFromTop(isMouseOver() ? 16 : 14).toFloat(), 2);

	g.setFont(isMouseOver() ? 13 : 12);
	g.setColour(Colours::white);
	g.drawFittedText(item->niceName, getLocalBounds().removeFromTop(isMouseOver() ? 16 : 14), Justification::centred, 3);
}


void LightBlockModelUI::updateThumbnail()
{
	if (item->customThumbnailPath.isNotEmpty())
	{
		modelImage = ImageCache::getFromFile(item->customThumbnailPath);
	}

	if (modelImage.getWidth() == 0)
	{
		int numBytes;
		const char* imgData = BinaryData::getNamedResource((StringUtil::toShortName(item->getTypeString()) + "_png").getCharPointer(), numBytes);
		modelImage = ImageCache::getFromMemory(imgData, numBytes);
	}


	repaint();
}


void LightBlockModelUI::mouseDown(const MouseEvent& e)
{
	BaseItemMinimalUI::mouseDown(e);

	if (e.mods.isLeftButtonDown())
	{
		if (e.mods.isAltDown())
		{
			for (auto& p : PropManager::getInstance()->items)
			{
				p->activeProvider->setValueFromTarget(item);
			}
		}
	}

}

void LightBlockModelUI::mouseDoubleClick(const MouseEvent& e)
{
	editBlock();
}

void LightBlockModelUI::addContextMenuItems(PopupMenu& m)
{
	PopupMenu menu;
	PopupMenu assignMenu;

	int index = 1;
	for (auto& p : PropManager::getInstance()->items)
	{
		assignMenu.addItem(index, p->niceName);
		index++;
	}

	m.addItem(-1, "Assign to all");
	m.addSubMenu("Assign to...", assignMenu);


}

void LightBlockModelUI::handleContextMenuResult(int result)
{
	if (result == -1)
	{
		for (auto& p : PropManager::getInstance()->items)
		{
			p->activeProvider->setValueFromTarget(item);
		}
	}if (result > 0 && result <= PropManager::getInstance()->items.size())
	{
		Prop* p = PropManager::getInstance()->items[result - 1];
		p->activeProvider->setValueFromTarget(item);
	};
}

void LightBlockModelUI::newMessage(const LightBlockModel::ModelEvent& e)
{
	switch (e.type)
	{
	case LightBlockModel::ModelEvent::CUSTOM_THUMBNAIL_CHANGED:
		updateThumbnail();
		break;

	case LightBlockModel::ModelEvent::EDITING_STATE_CHANGED:
		repaint();
		break;
	}
}

void LightBlockModelUI::controllableFeedbackUpdateInternal(Controllable* c)
{
	if (sequenceBlock != nullptr && c == sequenceBlock->sequence->isPlaying)
	{
		repaint();
	}
}
