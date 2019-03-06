/*
  ==============================================================================

    TimelineEditor.h
    Created: 17 Apr 2018 7:48:42pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "LightBlock/model/blocks/timeline/TimelineBlock.h"

class TimelineEditor :
	public TimeMachineView
{
public:
	TimelineEditor(const String &contentName = "Timeline Editor");
	~TimelineEditor();

	//virtual void inspectablesSelectionChanged() override;

	static TimelineEditor * create(const String &contentName) { return new TimelineEditor(contentName); }
};