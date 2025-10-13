/*
  ==============================================================================

	PropManagerUI.cpp
	Created: 10 Apr 2018 7:00:09pm
	Author:  Ben

  ==============================================================================
*/

#include "Prop/PropIncludes.h"

PropManagerUI::PropManagerUI(const String& name, PropManager* m) :
	ManagerShapeShifterUI(name, m)
{
	headerSize = 150;

	noItemText = "Start by adding props by right clicking here, or when the props are powered on and connected, left click here and hit auto detect on the Inspector";
	setDefaultLayout(HORIZONTAL);
	addExistingItems();

	autoDetectUI.reset(manager->detectProps->createButtonUI());
	assignPropIdUI.reset(manager->assignPropIdTrigger->createButtonUI());
	autoAssignUI.reset(manager->autoAssignIdTrigger->createButtonUI());

	enableUI.reset(manager->enableAll->createButtonUI());
	disableUI.reset(manager->disableAll->createButtonUI());
	restartUI.reset(manager->resetAll->createButtonUI());
	powerOffUI.reset(manager->powerOffAll->createButtonUI());
	saveSettingsUI.reset(manager->saveAll->createButtonUI());
	showBatteryUI.reset(manager->showBattery->createToggle());
	globalBrightnessUI.reset(manager->globalBrightness->createSlider());


	uploadAllUI.reset(manager->uploadAll->createButtonUI());
	playbackModeUI.reset(manager->playbackMode->createToggle());

	addAndMakeVisible(autoDetectUI.get());
	addAndMakeVisible(assignPropIdUI.get());
	addAndMakeVisible(autoAssignUI.get());
	addAndMakeVisible(uploadAllUI.get());
	addAndMakeVisible(playbackModeUI.get());
	addAndMakeVisible(showBatteryUI.get());

	addAndMakeVisible(enableUI.get());
	addAndMakeVisible(disableUI.get());
	addAndMakeVisible(restartUI.get());
	addAndMakeVisible(powerOffUI.get());
	addAndMakeVisible(saveSettingsUI.get());
	addAndMakeVisible(globalBrightnessUI.get());
}

PropManagerUI::~PropManagerUI()
{
}

void PropManagerUI::resizedInternalHeader(Rectangle<int>& r)
{
	Rectangle<int> addR = r.removeFromTop(24).removeFromRight(24);
	ManagerUI::resizedInternalHeader(addR);
	Rectangle<int> hr = r.reduced(2);
	autoDetectUI->setBounds(hr.removeFromTop(20));
	hr.removeFromTop(2);
	assignPropIdUI->setBounds(hr.removeFromTop(20));
	hr.removeFromTop(2);
	autoAssignUI->setBounds(hr.removeFromTop(20));
	hr.removeFromTop(8);
	enableUI->setBounds(hr.removeFromTop(20));
	hr.removeFromTop(2);
	disableUI->setBounds(hr.removeFromTop(20));
	hr.removeFromTop(2);
	restartUI->setBounds(hr.removeFromTop(20));
	hr.removeFromTop(2);
	powerOffUI->setBounds(hr.removeFromTop(20));
	hr.removeFromTop(2);
	saveSettingsUI->setBounds(hr.removeFromTop(20));
	hr.removeFromTop(8);
	globalBrightnessUI->setBounds(hr.removeFromTop(20));
	hr.removeFromTop(2);
	showBatteryUI->setBounds(hr.removeFromTop(20));

	//hr.removeFromBottom(2);
	uploadAllUI->setBounds(hr.removeFromBottom(20));
	hr.removeFromBottom(2);
	playbackModeUI->setBounds(hr.removeFromBottom(20));
}

void PropManagerUI::showMenuAndAddItem(bool fromAddButton, Point<int> mouseDownPos)
{
	if (fromAddButton)
	{
		ManagerShapeShifterUI::showMenuAndAddItem(fromAddButton, mouseDownPos, nullptr);
		return;
	}

	PopupMenu menu;
	menu.addSubMenu("Create", manager->managerFactory->getMenu());
	menu.addItem(-3, "Auto Detect Props");
	menu.addItem(-2, "Assign IDs from Props");
	menu.addItem(-1, "Auto assign IDs");

	PopupMenu assignToAllMenu;
	Array<LightBlockColorProvider*> mList = LightBlockModelLibrary::fillProvidersMenu(assignToAllMenu, true, true, false, 10000);

	menu.addSubMenu("Assign to All", assignToAllMenu);

	menu.showMenuAsync(PopupMenu::Options(), [this, mList](int result)
		{
			if (result == 0) return;
			else if (result == -1) manager->autoAssignIdTrigger->trigger();
			else if (result == -2) manager->assignPropIdTrigger->trigger();
			else if (result == -3) manager->detectProps->trigger();
			else if (result >= 10000)
			{
				LightBlockColorProvider* mp = mList[result - 10000];
				for (auto& p : manager->items) p->activeProvider->setValueFromTarget(mp);
			}
			else
			{
				Prop* p = manager->managerFactory->createFromMenuResult(result);
				if (p != nullptr) manager->addItem(p);
			}
		}
	);
}
