/*
  ==============================================================================

    PropManagerUI.cpp
    Created: 10 Apr 2018 7:00:09pm
    Author:  Ben

  ==============================================================================
*/

#include "PropManagerUI.h"
#include "LightBlock/model/LightBlockModelLibrary.h"

PropManagerUI::PropManagerUI(const String &name, PropManager * m) :
	BaseManagerShapeShifterUI(name, m)
{
	noItemText = "Start by adding props by right clicking here, or when the props are powered on and connected, left click here and hit auto detect on the Inspector";
	setDefaultLayout(HORIZONTAL);
	addExistingItems();

	autoDetectUI.reset(manager->detectProps->createButtonUI());
	autoAssignUI.reset(manager->autoAssignIdTrigger->createButtonUI());
	bakeAllUI.reset(manager->bakeAll->createButtonUI());
	bakeModeUI.reset(manager->bakeMode->createToggle());

	addAndMakeVisible(autoDetectUI.get());
	addAndMakeVisible(autoAssignUI.get());
	addAndMakeVisible(bakeAllUI.get());
	addAndMakeVisible(bakeModeUI.get());
}

PropManagerUI::~PropManagerUI()
{
}

void PropManagerUI::resizedInternalHeader(Rectangle<int>& r)
{
	Rectangle<int> hr = r.removeFromRight(100).reduced(2);
	autoAssignUI->setBounds(hr.removeFromTop(20));
	hr.removeFromTop(2);
	autoDetectUI->setBounds(hr.removeFromTop(20));
	hr.removeFromTop(2);
	bakeAllUI->setBounds(hr.removeFromTop(20));
	hr.removeFromTop(2);
	bakeModeUI->setBounds(hr.removeFromTop(20));
}

void PropManagerUI::showMenuAndAddItem(bool fromAddButton, Point<int> mouseDownPos)
{
	if (fromAddButton)
	{
		BaseManagerShapeShifterUI::showMenuAndAddItem(fromAddButton, mouseDownPos);
		return;
	}

	PopupMenu menu;
	menu.addSubMenu("Create", manager->managerFactory->getMenu());
	menu.addItem(-2, "Auto Detect Props");
	menu.addItem(-1, "Auto assign IDs");

	PopupMenu assignToAllMenu;
	Array<LightBlockColorProvider *> mList =  LightBlockModelLibrary::fillProvidersMenu(assignToAllMenu, true, 10000);

	menu.addSubMenu("Assign to All", assignToAllMenu);
	int result = menu.show();

	if (result == 0) return;
	else if (result == -1) 	manager->autoAssignIdTrigger->trigger();
	else if (result == -2) manager->detectProps->trigger();
	else if (result >= 10000)
	{
		LightBlockColorProvider * mp = mList[result - 10000];
		for (auto & p : manager->items) p->activeProvider->setValueFromTarget(mp);
	} else
	{
		Prop * p = manager->managerFactory->createFromMenuResult(result);
		if(p != nullptr) manager->addItem(p);
	}
}
