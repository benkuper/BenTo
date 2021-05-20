/*
  ==============================================================================

    TimelineEditor.h
    Created: 17 Apr 2018 7:48:42pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

class TimelineEditor :
	public TimeMachineView,
	public EngineListener
{
public:
	TimelineEditor(const String &contentName = TimelineEditor::getTypeStringStatic());
	~TimelineEditor();

	void endLoadFile() override;
	
	static const String getTypeStringStatic()  { return "Sequence Editor"; }
	static TimelineEditor * create(const String &contentName) { return new TimelineEditor(contentName); }
};