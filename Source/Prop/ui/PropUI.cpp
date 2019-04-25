/*
  ==============================================================================

    PropUI.cpp
    Created: 10 Apr 2018 7:00:05pm
    Author:  Ben

  ==============================================================================
*/

#include "PropUI.h"
#include "LightBlock/model/LightBlockModelLibrary.h"
#include "LightBlock/model/ui/LightBlockModelUI.h"

PropUI::PropUI(Prop * p) :
	BaseItemUI(p),
	viz(p)
{

	acceptedDropTypes.add("LightBlockModel");

	idUI = p->globalID->createStepper();
	idUI->showLabel = true;
	targetUI = p->activeProvider->createTargetUI();
	batteryUI = p->battery->createSlider();
	batteryUI->showLabel = false;

	addAndMakeVisible(targetUI);
	addAndMakeVisible(idUI);
	addAndMakeVisible(batteryUI); 
	addAndMakeVisible(&viz);

	setSize(100, 100);
	
}

PropUI::~PropUI()
{
}

void PropUI::paintOverChildren(Graphics & g)
{
	BaseItemUI::paintOverChildren(g);
	if (item->isBaking->boolValue())
	{
		g.fillAll(Colours::black.withAlpha(.3f));
		
		g.setColour(Colours::orange.darker().withAlpha(.2f));
		g.fillRoundedRectangle(viz.getBounds().removeFromBottom(item->bakingProgress->floatValue()*viz.getHeight()).toFloat(), 2);
		
		g.setColour(Colours::limegreen.darker().withAlpha(.2f));
		g.fillRoundedRectangle(viz.getBounds().removeFromBottom(item->uploadProgress->floatValue()*viz.getHeight()).toFloat(), 2);

		g.setColour(item->isUploading->boolValue() ? Colours::limegreen : Colours::orange);
		g.drawFittedText(item->isUploading->boolValue()?"Uploading ...":"Baking...", getLocalBounds(), Justification::centred, 1);
	}

	if (isDraggingOver)
	{
		g.fillAll(BLUE_COLOR.withAlpha(.3f));
	}
}

void PropUI::mouseDown(const MouseEvent & e)
{
	BaseItemUI::mouseDown(e);

	if (e.mods.isRightButtonDown())
	{
		LightBlockColorProvider * p = LightBlockModelLibrary::showProvidersAndGet();
		if (p != nullptr) item->activeProvider->setValueFromTarget(p);
	} else if (e.mods.isLeftButtonDown())
	{
		if (e.mods.isAltDown())
		{
			item->findPropMode->setValue(true);
		}
	}
}

void PropUI::mouseUp(const MouseEvent & e)
{
	BaseItemUI::mouseUp(e);
	item->findPropMode->setValue(false);
}

void PropUI::resizedInternalHeader(Rectangle<int>& r)
{
	r.removeFromRight(2);
	idUI->setBounds(r.removeFromRight(60));
}

void PropUI::resizedInternalContent(Rectangle<int> &r)
{
	targetUI->setBounds(r.removeFromTop(16));
	batteryUI->setBounds(r.removeFromTop(10));
	r.removeFromTop(2);
	viz.setBounds(r.reduced(2));
}

void PropUI::controllableFeedbackUpdateInternal(Controllable * c)
{
	if (c == item->isBaking || c == item->bakingProgress || c == item->isUploading || c == item->uploadProgress) repaint();
}

void PropUI::itemDropped(const SourceDetails & source)
{
	LightBlockModelUI * modelUI = dynamic_cast<LightBlockModelUI *>(source.sourceComponent.get());

	if (modelUI != nullptr)
	{
		LightBlockColorProvider * provider = modelUI->item;

		bool shift = KeyPress::isKeyCurrentlyDown(16);
		if (shift)
		{
			PopupMenu m;
			m.addItem(-1, "Default");
			m.addSeparator();
			int index = 1;
			for (auto &p : modelUI->item->presetManager.items) m.addItem(index++, p->niceName);
			int result = m.show();
			if (result >= 1) provider = modelUI->item->presetManager.items[result - 1];
		}

		item->activeProvider->setValueFromTarget(provider);
	}
	
	BaseItemUI::itemDropped(source);

}
