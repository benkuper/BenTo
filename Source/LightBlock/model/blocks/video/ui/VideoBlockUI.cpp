/*
  ==============================================================================

    VideoBlockUI.cpp
    Created: 23 Apr 2018 9:45:30pm
    Author:  Ben

  ==============================================================================
*/


VideoBlockUI::VideoBlockUI(VideoBlock * block) :
	LightBlockModelUI(block),
	vb(block)
{
	liveUI.reset(vb->inputIsLive->createToggle());
	addAndMakeVisible(liveUI.get());
	resized();
}

VideoBlockUI::~VideoBlockUI()
{
}

void VideoBlockUI::paint(Graphics & g)
{
	LightBlockModelUI::paint(g);
}

void VideoBlockUI::resized()
{
	liveUI->setBounds(getLocalBounds().withSize(10, 10).withCentre(Point<int>(getWidth()-10, 10)));
}

void VideoBlockUI::editBlock()
{
	ShapeShifterManager::getInstance()->showContent(SpatializerPanel::getTypeStringStatic());
}
