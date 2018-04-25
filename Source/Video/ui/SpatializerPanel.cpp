/*
  ==============================================================================

	SpatializerPanel.cpp
	Created: 23 Apr 2018 9:42:51pm
	Author:  Ben

  ==============================================================================
*/

#include "SpatializerPanel.h"

SpatializerPanel::SpatializerPanel(const String & name) :
	BaseManagerShapeShifterUI(name, Spatializer::getInstance(),false)
{
	addExistingItems();
	startTimerHz(30); //repaint at 30hz
}


SpatializerPanel::~SpatializerPanel()
{
}


void SpatializerPanel::paint(Graphics & g)
{
	if (OpenGLContext::getCurrentContext() == nullptr)
	{
		g.setColour(HIGHLIGHT_COLOR);
		g.setFont(16);
		g.drawFittedText("This panel must be attached to the main window in order work properly", getLocalBounds().reduced(10), Justification::centred, 4);
		return;
	}
	if (Spatializer::getInstance()->showTexture->boolValue() && manager->videoBlock != nullptr && manager->videoBlock->inputIsLive->boolValue())
	{
		g.setColour(Colours::white);
		g.drawImage(manager->videoBlock->receiver->getImage(), getLocalBounds().toFloat());
	}
}

void SpatializerPanel::resized()
{
	for (auto &i : itemsUI)
	{
		i->updateBounds();
	}
}

void SpatializerPanel::newMessage(const ContainerAsyncEvent & e)
{
	if (e.type == ContainerAsyncEvent::ControllableFeedbackUpdate)
	{
		if (e.targetControllable == manager->showTexture) repaint();
	}
}

void SpatializerPanel::timerCallback()
{
	if (manager->videoBlock == nullptr || !manager->videoBlock->inputIsLive->boolValue()) return;
	repaint();
}

SpatItemUI * SpatializerPanel::createUIForItem(SpatItem * i)
{
	return new SpatItemUI(i, this);
}

Point<float> SpatializerPanel::getPositionForRelative(Point<float> relPos)
{
	return Point<float>(relPos.x * getWidth(), relPos.y * getHeight());
}

Point<float> SpatializerPanel::getRelativeForPosition(Point<float> absolutePos)
{
	return Point<float>(absolutePos.x / getWidth(), absolutePos.y / getHeight());
}