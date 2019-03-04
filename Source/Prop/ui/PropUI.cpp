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
	viz(p),
	acceptModelDrop(true),
	isDraggingItemOver(false)
{
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
		g.fillAll(Colours::black.withAlpha(.5f));
		g.setColour(Colours::orange);
		g.drawFittedText("Baking ...", getLocalBounds(), Justification::centred, 1);
	}

	if (isDraggingItemOver)
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
	if (c == item->isBaking) repaint();
}

bool PropUI::isInterestedInDragSource(const SourceDetails & source)
{
	return source.description == "LightBlockModel";
}

void PropUI::itemDragEnter(const SourceDetails & source)
{
	isDraggingItemOver = false;
	repaint();
}

void PropUI::itemDragExit(const SourceDetails & source)
{
	isDraggingItemOver = false;
	repaint();
}

void PropUI::itemDropped(const SourceDetails & source)
{
	LightBlockModelUI * modelUI = dynamic_cast<LightBlockModelUI *>(source.sourceComponent.get());
	if (modelUI != nullptr) item->activeProvider->setValueFromTarget(modelUI->item);
}
