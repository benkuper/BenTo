/*
  ==============================================================================

	BentoSequenceEditor.cpp
	Created: 17 Apr 2018 7:48:42pm
	Author:  Ben

  ==============================================================================
*/

#include "Sequence/SequenceIncludes.h"

BentoSequenceEditor::BentoSequenceEditor(const String &contentName) :
	TimeMachineView(contentName)
{
	Engine::mainEngine->addEngineListener(this);
	BentoSequenceBlock * b = InspectableSelectionManager::mainSelectionManager->getInspectableAs<BentoSequenceBlock>();
	if (b != nullptr) setSequence(b->sequence.get());
}

BentoSequenceEditor::~BentoSequenceEditor()
{
	if (Engine::mainEngine != nullptr) Engine::mainEngine->removeEngineListener(this);
}

void BentoSequenceEditor::endLoadFile()
{
	for (auto &i : LightBlockModelLibrary::getInstance()->sequenceBlocks.items)
	{
		if (i->isBeingEdited)
		{
			setSequence(((BentoSequenceBlock *)i)->sequence.get());
			break;
		}
	}
}
