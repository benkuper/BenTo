/*
  ==============================================================================

    PropManagerUI.h
    Created: 10 Apr 2018 7:00:09pm
    Author:  Ben

  ==============================================================================
*/

#pragma once


class PropManagerUI :
	public BaseManagerShapeShifterUI<PropManager, Prop, PropUI>
{
public:
	PropManagerUI(const String &name, PropManager * manager);
	~PropManagerUI();

	std::unique_ptr<TriggerButtonUI> autoDetectUI;
	std::unique_ptr<TriggerButtonUI> autoAssignUI;
	std::unique_ptr<TriggerButtonUI> enableUI;
	std::unique_ptr<TriggerButtonUI> disableUI;
	std::unique_ptr<FloatSliderUI> globalBrightnessUI;
	std::unique_ptr<TriggerButtonUI> uploadAllUI;
	std::unique_ptr<BoolToggleUI> playbackModeUI;

	void resizedInternalHeader(Rectangle<int> &r) override;
	void showMenuAndAddItem(bool fromAddButton, Point<int> mouseDownPos) override;

	static PropManagerUI * create(const String &contentName) { return new PropManagerUI(contentName, PropManager::getInstance()); }
};