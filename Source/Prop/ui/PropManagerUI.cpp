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
	setDefaultLayout(HORIZONTAL);
}

PropManagerUI::~PropManagerUI()
{
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
	menu.addItem(-1, "Auto assign IDs");

	PopupMenu assignToAllMenu;
	Array<LightBlockColorProvider *> mList =  LightBlockModelLibrary::fillProvidersMenu(assignToAllMenu, true, 10000);

	menu.addSubMenu("Assign to All", assignToAllMenu);
	int result = menu.show();

	if (result == 0) return;
	else if (result == -1)
	{
		manager->autoAssignIdTrigger->trigger();
	}if (result >= 10000)
	{
		LightBlockColorProvider * mp = mList[result - 10000];
		for (auto & p : manager->items) p->activeProvider->setValueFromTarget(mp);
	} else
	{
		Prop * p = manager->managerFactory->createFromMenuResult(result);
		if(p != nullptr) manager->addItem(p);
	}
}
