/*
  ==============================================================================

	LightBlockModelUI.cpp
	Created: 10 Apr 2018 8:10:04pm
	Author:  Ben

  ==============================================================================
*/

LightBlockModelUI::LightBlockModelUI(LightBlockModel* model) :
	BaseItemMinimalUI(model),
	timelineBlock(dynamic_cast<TimelineBlock*>(model))
{
	bgColor = item->isBeingEdited ? BLUE_COLOR.darker().withSaturation(.3f) : bgColor = BG_COLOR.brighter(.1f);

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
	Colour bgC = bgColor;
	if (timelineBlock != nullptr && timelineBlock->sequence->isPlaying->boolValue()) bgC = GREEN_COLOR.darker();
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
	else if (e.mods.isRightButtonDown())
	{
		PopupMenu menu;
		PopupMenu assignMenu;

		int index = 1;
		for (auto& p : PropManager::getInstance()->items)
		{
			assignMenu.addItem(index, p->niceName);
			index++;
		}

		menu.addItem(-1, "Assign to all");
		menu.addSubMenu("Assign to...", assignMenu);

		menu.showMenuAsync(PopupMenu::Options(), [this](int result)
			{
				if (result == 0) return;
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
				}
			}
		);
	}
}

void LightBlockModelUI::mouseDoubleClick(const MouseEvent& e)
{
	editBlock();
}

void LightBlockModelUI::newMessage(const LightBlockModel::ModelEvent& e)
{
	switch (e.type)
	{
	case LightBlockModel::ModelEvent::CUSTOM_THUMBNAIL_CHANGED:
		updateThumbnail();
		break;

	case LightBlockModel::ModelEvent::EDITING_STATE_CHANGED:
		bgColor = item->isBeingEdited ? BLUE_COLOR.darker().withSaturation(.3f) : bgColor = BG_COLOR.brighter(.1f);
		repaint();
		break;
	}
}

void LightBlockModelUI::controllableFeedbackUpdateInternal(Controllable* c)
{
	if (timelineBlock != nullptr && c == timelineBlock->sequence->isPlaying)
	{
		repaint();
	}
}
