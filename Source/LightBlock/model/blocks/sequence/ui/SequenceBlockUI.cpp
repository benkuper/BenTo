/*
  ==============================================================================

	BentoSequenceBlockUI.cpp
	Created: 23 Apr 2018 11:36:00am
	Author:  Ben

  ==============================================================================
*/

#include "LightBlock/LightBlockIncludes.h"
#include "Sequence/SequenceIncludes.h"

BentoSequenceBlockUI::BentoSequenceBlockUI(BentoSequenceBlock* block) :
	LightBlockModelUI(block),
	tb(block)
{
}

BentoSequenceBlockUI::~BentoSequenceBlockUI()
{
}

void BentoSequenceBlockUI::editBlock()
{
	TimeMachineView* view = dynamic_cast<TimeMachineView*>(ShapeShifterManager::getInstance()->showContent(BentoSequenceEditor::getTypeStringStatic()));
	if (view != nullptr) view->setSequence(tb->sequence.get());
}
