/*
  ==============================================================================

    SharedTextureBlockUI.cpp
    Created: 23 Apr 2018 9:45:30pm
    Author:  Ben

  ==============================================================================
*/

SharedTextureBlockUI::SharedTextureBlockUI(SharedTextureBlock * block) :
	LightBlockModelUI(block),
	vb(block)
{
	liveUI.reset(vb->inputIsLive->createToggle());
	addAndMakeVisible(liveUI.get());
	resized();
}

SharedTextureBlockUI::~SharedTextureBlockUI()
{
}

void SharedTextureBlockUI::paint(Graphics & g)
{
	LightBlockModelUI::paint(g);
}

void SharedTextureBlockUI::resized()
{
	liveUI->setBounds(getLocalBounds().withSize(10, 10).withCentre(Point<int>(getWidth()-10, 10)));
}

void SharedTextureBlockUI::editBlock()
{
	ShapeShifterManager::getInstance()->showContent(SpatializerPanel::getTypeStringStatic());
}
