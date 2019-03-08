/*
  ==============================================================================

	TimelineEditor.cpp
	Created: 17 Apr 2018 7:48:42pm
	Author:  Ben

  ==============================================================================
*/

#include "TimelineEditor.h"
#include "LightBlock/model/LightBlockModelLibrary.h"

TimelineEditor::TimelineEditor(const String &contentName) :
	TimeMachineView(contentName)
{
	Engine::mainEngine->addEngineListener(this);
	TimelineBlock * b = InspectableSelectionManager::mainSelectionManager->getInspectableAs<TimelineBlock>();
	if (b != nullptr) setSequence(&b->sequence);
}

TimelineEditor::~TimelineEditor()
{
	if (Engine::mainEngine != nullptr) Engine::mainEngine->removeEngineListener(this);
}

void TimelineEditor::endLoadFile()
{
	for (auto &i : LightBlockModelLibrary::getInstance()->timelineBlocks.items)
	{
		if (i->isBeingEdited)
		{
			setSequence(&((TimelineBlock *)i)->sequence);
			break;
		}
	}
}
