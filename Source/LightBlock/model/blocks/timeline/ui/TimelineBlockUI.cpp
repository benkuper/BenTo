/*
  ==============================================================================

	TimelineBlockUI.cpp
	Created: 23 Apr 2018 11:36:00am
	Author:  Ben

  ==============================================================================
*/

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
	TimeMachineView* view = dynamic_cast<TimeMachineView*>(ShapeShifterManager::getInstance()->showContent(TimelineEditor::getTypeStringStatic()));
	if (view != nullptr) view->setSequence(tb->sequence.get());
}
