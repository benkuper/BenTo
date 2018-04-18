/*
  ==============================================================================

	TimelineEditor.cpp
	Created: 17 Apr 2018 7:48:42pm
	Author:  Ben

  ==============================================================================
*/

#include "TimelineEditor.h"

TimelineEditor::TimelineEditor(const String &contentName) :
	TimeMachineView(contentName)
{
}

TimelineEditor::~TimelineEditor()
{

}

void TimelineEditor::inspectablesSelectionChanged()
{
	TimelineBlock * b = InspectableSelectionManager::mainSelectionManager->getInspectableAs<TimelineBlock>();
	if (b != nullptr) setSequence(&b->sequence);
}
