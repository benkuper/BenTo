/*
  ==============================================================================

    LiveFeedBlockUI.cpp
    Created: 23 Apr 2018 9:45:30pm
    Author:  Ben

  ==============================================================================
*/

LiveFeedBlockUI::LiveFeedBlockUI(LiveFeedBlock * block) :
	LightBlockModelUI(block),
	vb(block)
{
	liveUI.reset(vb->inputIsLive->createToggle());
	addAndMakeVisible(liveUI.get());
	resized();
}

LiveFeedBlockUI::~LiveFeedBlockUI()
{
}

void LiveFeedBlockUI::paint(Graphics & g)
{
	LightBlockModelUI::paint(g);
}

void LiveFeedBlockUI::resized()
{
	liveUI->setBounds(getLocalBounds().withSize(10, 10).withCentre(Point<int>(getWidth()-10, 10)));
}

void LiveFeedBlockUI::editBlock()
{
	ShapeShifterManager::getInstance()->showContent(SpatializerPanel::getTypeStringStatic());
}
