/*
  ==============================================================================

	TimelineBlockUI.cpp
	Created: 23 Apr 2018 11:36:00am
	Author:  Ben

  ==============================================================================
*/

#include "TimelineBlockUI.h"
#include "Timeline/TimelineEditor.h"

TimelineBlockUI::TimelineBlockUI(TimelineBlock * block) :
	LightBlockModelUI(block),
	tb(block)
{
}

TimelineBlockUI::~TimelineBlockUI()
{
}

void TimelineBlockUI::editBlock()
{
	ShapeShifterManager::getInstance()->showContent("Timeline Editor");
}
