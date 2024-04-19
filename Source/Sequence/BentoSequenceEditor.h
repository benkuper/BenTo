/*
  ==============================================================================

    BentoSequenceEditor.h
    Created: 17 Apr 2018 7:48:42pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

class BentoSequenceEditor :
	public TimeMachineView,
	public EngineListener
{
public:
	BentoSequenceEditor(const String &contentName = BentoSequenceEditor::getTypeStringStatic());
	~BentoSequenceEditor();

	void endLoadFile() override;
	
	static const String getTypeStringStatic()  { return "Sequence Editor"; }
	static BentoSequenceEditor * create(const String &contentName) { return new BentoSequenceEditor(contentName); }
};