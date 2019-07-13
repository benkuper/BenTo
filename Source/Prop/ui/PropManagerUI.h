/*
  ==============================================================================

    PropManagerUI.h
    Created: 10 Apr 2018 7:00:09pm
    Author:  Ben

  ==============================================================================
*/

#pragma once


#include "../PropManager.h"
#include "PropUI.h"

class PropManagerUI :
	public BaseManagerShapeShifterUI<PropManager, Prop, PropUI>
{
public:
	PropManagerUI(const String &name, PropManager * manager);
	~PropManagerUI();

	std::unique_ptr<TriggerButtonUI> autoDetectUI;
	std::unique_ptr<TriggerButtonUI> autoAssignUI;
	std::unique_ptr<TriggerButtonUI> bakeAllUI;
	std::unique_ptr<BoolToggleUI> bakeModeUI;

	void resizedInternalHeader(Rectangle<int> &r) override;
	void showMenuAndAddItem(bool fromAddButton, Point<int> mouseDownPos) override;

	static PropManagerUI * create(const String &contentName) { return new PropManagerUI(contentName, PropManager::getInstance()); }
};