/*
  ==============================================================================

    PropUI.cpp
    Created: 10 Apr 2018 7:00:05pm
    Author:  Ben

  ==============================================================================
*/

#include "PropUI.h"
#include "LightBlock/model/LightBlockModelLibrary.h"

PropUI::PropUI(Prop * p) :
	BaseItemUI(p),
	viz(p)
{
	idUI = p->id->createStepper();
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