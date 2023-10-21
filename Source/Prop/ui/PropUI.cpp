/*
  ==============================================================================

	PropUI.cpp
	Created: 10 Apr 2018 7:00:05pm
	Author:  Ben

  ==============================================================================
*/

#include "Prop/PropIncludes.h"

PropUI::PropUI(Prop* p) :
	BaseItemUI(p, HORIZONTAL),
	viz(p)
{
	itemLabel.setVisible(false);

	acceptedDropTypes.add("LightBlockModel");
	acceptedDropTypes.add(SequenceBlock::getTypeStringStatic());
	acceptedDropTypes.add(StreamingScriptBlock::getTypeStringStatic());
	acceptedDropTypes.add(EmbeddedScriptBlock::getTypeStringStatic());
	acceptedDropTypes.add(PictureBlock::getTypeStringStatic());
	acceptedDropTypes.add(NodeBlock::getTypeStringStatic());
	acceptedDropTypes.add(VideoFileBlock::getTypeStringStatic());
	acceptedDropTypes.add(SharedTextureBlock::getTypeStringStatic());

	idUI.reset(p->globalID->createLabelUI());
	idUI->showLabel = false;

	if (item->battery != nullptr)
	{
		batteryUI.reset(item->battery->createSlider());
		addAndMakeVisible(batteryUI.get());
	}

	addAndMakeVisible(idUI.get());
	addAndMakeVisible(&viz);


	viz.setInterceptsMouseClicks(false, false);

	Prop::Shape shape = p->shape->getValueDataAsEnum<Prop::Shape>();

	setSize(shape == Prop::HOOP ? 100 : 50, 100);
}

PropUI::~PropUI()
{
}

void PropUI::paintOverChildren(Graphics& g)
{
	BaseItemUI::paintOverChildren(g);
	if (item->isGeneratingPlayback->boolValue())
	{
		g.fillAll(Colours::black.withAlpha(.3f));

		g.setColour(Colours::orange.darker().withAlpha(.2f));
		g.fillRoundedRectangle(viz.getBounds().removeFromBottom(item->playbackGenProgress->floatValue() * viz.getHeight()).toFloat(), 2);

		g.setColour(Colours::limegreen.darker().withAlpha(.2f));
		g.fillRoundedRectangle(viz.getBounds().removeFromBottom(item->uploadProgress->floatValue() * viz.getHeight()).toFloat(), 2);

		g.setColour(item->isUploading->boolValue() ? Colours::limegreen : Colours::orange);
		g.drawFittedText(item->isUploading->boolValue() ? "Uploading ..." : "Baking...", getLocalBounds(), Justification::centred, 1);
	}
	//else if (item->isFlashing->boolValue())
	//{
	//	g.setColour(Colours::lightpink.withAlpha(.2f));
	//	g.fillRoundedRectangle(viz.getBounds().removeFromBottom(item->flashingProgression->floatValue() * viz.getHeight()).toFloat(), 2);

	//	g.setColour(Colours::lightpink);
	//	g.drawFittedText("Flashing...", getLocalBounds(), Justification::centred, 1);
	//}

	g.setColour(item->isConnected->boolValue() ? GREEN_COLOR : BG_COLOR);
	Rectangle<int> r = getMainBounds().translated(0, headerHeight + headerGap).removeFromRight(20).removeFromTop(20).reduced(1);
	g.fillEllipse(r.toFloat().reduced(4));

	if (item->motionRef != nullptr && item->motionRef->boolValue())
	{
		g.setColour(YELLOW_COLOR);
		g.drawEllipse(r.toFloat().reduced(2), 1);
	}


	if (isDraggingOver)
	{
		g.fillAll(BLUE_COLOR.withAlpha(.3f));
	}
}

void PropUI::mouseDown(const MouseEvent& e)
{
	BaseItemUI::mouseDown(e);

	if (e.mods.isLeftButtonDown())
	{
		if (e.mods.isAltDown())
		{
			item->findPropMode->setValue(true);
		}
	}
}

void PropUI::mouseUp(const MouseEvent& e)
{
	BaseItemUI::mouseUp(e);
	item->findPropMode->setValue(false);
}

void PropUI::addContextMenuItems(PopupMenu& m)
{
	PopupMenu controlMenu;
	controlMenu.addItem("Upload Playback", [this]() { item->uploadPlaybackTrigger->trigger(); });
	controlMenu.addItem("Power Off", [this]() { item->powerOffTrigger->trigger(); });
	controlMenu.addItem("Restart", [this]() { item->restartTrigger->trigger(); });

	m.addSubMenu("Control", controlMenu);
}

void PropUI::resizedInternalHeader(Rectangle<int>& r)
{

}

void PropUI::resizedInternalContent(Rectangle<int>& r)
{

	idUI->setBounds(r.removeFromTop(16).removeFromLeft(30));
	r.removeFromTop(2);

	if (batteryUI != nullptr)
	{
		batteryUI->setBounds(r.removeFromTop(14).reduced(2));
	}

	viz.setBounds(r.reduced(2));
}

void PropUI::controllableFeedbackUpdateInternal(Controllable* c)
{
	if (c == item->isGeneratingPlayback || c == item->playbackGenProgress || c == item->isUploading || c == item->uploadProgress || c == item->isConnected || c == item->motionRef /*|| c == item->isFlashing || c == item->flashingProgression*/) repaint();
	else if (c == item->shape)
	{
		Prop::Shape shape = item->shape->getValueDataAsEnum<Prop::Shape>();
		setSize(shape == Prop::HOOP ? 100 : 50, 100);
	}
}

void PropUI::itemDropped(const SourceDetails& source)
{
	LightBlockModelUI* modelUI = dynamic_cast<LightBlockModelUI*>(source.sourceComponent.get());

	if (modelUI != nullptr)
	{

		bool shift = KeyPress::isKeyCurrentlyDown(16);
		if (shift)
		{
			PopupMenu m;
			m.addItem(-1, "Default");
			m.addSeparator();
			int index = 1;
			for (auto& p : modelUI->item->presetManager.items) m.addItem(index++, p->niceName);
			m.showMenuAsync(PopupMenu::Options(), [this, modelUI](int result)
				{
					if (result >= 1)
					{
						LightBlockColorProvider* provider = modelUI->item->presetManager.items[result - 1];
						item->activeProvider->setValueFromTarget(provider);
					}
				}
			);
		}
		else
		{
			item->activeProvider->setValueFromTarget(modelUI->item);
		}

	}

	BaseItemUI::itemDropped(source);

}
