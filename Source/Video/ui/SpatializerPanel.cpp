/*
  ==============================================================================

	SpatializerPanel.cpp
	Created: 23 Apr 2018 9:42:51pm
	Author:  Ben

  ==============================================================================
*/

#include "Video/VideoIncludes.h"
#include "LightBlock/LightBlockIncludes.h"

SpatializerPanel::SpatializerPanel(const String & name) :
	ShapeShifterContentComponent(name),
	needsRepaint(true),
	videoBlock(nullptr)
{
	
	setCurrentLayoutView(Spatializer::getInstance()->currentLayout);
	startTimerHz(30); //repaint at 30hz

	Spatializer::getInstance()->addAsyncSpatListener(this);
}


SpatializerPanel::~SpatializerPanel()
{
	Spatializer::getInstance()->removeAsyncSpatListener(this);
}


void SpatializerPanel::setCurrentLayoutView(SpatLayout * layout)
{
	if(currentLayoutView != nullptr && layout == currentLayoutView->layout) return;
	if (currentLayoutView != nullptr)
	{
		removeChildComponent(currentLayoutView.get());
		currentLayoutView = nullptr;
	}
	
	if(layout != nullptr)
	{
		currentLayoutView.reset(new SpatLayoutView(Spatializer::getInstance(), layout));
		addAndMakeVisible(currentLayoutView.get());
		resized();
	}
}

void SpatializerPanel::setVideoBlock(VideoBlock* b)
{
	videoBlock = b;
}

void SpatializerPanel::paint(Graphics & g)
{
	if (Engine::mainEngine->isClearing) return;

	if (juce::OpenGLContext::getCurrentContext() == nullptr)
	{
		g.setColour(HIGHLIGHT_COLOR);
		g.setFont(16);
		g.drawFittedText("This panel must be attached to the main window in order work properly", getLocalBounds().reduced(10), Justification::centred, 4);
		return;
	}

	if (videoBlock != nullptr && videoBlock->inputIsLive->boolValue())
	{
		g.setColour(Colours::white.withAlpha(Spatializer::getInstance()->textureOpacity->floatValue()));
		g.drawImage(videoBlock->getImage(), getLocalBounds().toFloat());
	}
}

void SpatializerPanel::resized()
{
	if (currentLayoutView != nullptr)
	{
		currentLayoutView->setBounds(getLocalBounds());
	}
}

void SpatializerPanel::mouseDown(const MouseEvent& e)
{
	if (e.eventComponent == this)
	{
		Spatializer::getInstance()->selectThis();
	}
}

void SpatializerPanel::newMessage(const ContainerAsyncEvent & e)
{
	if (e.type == ContainerAsyncEvent::ControllableFeedbackUpdate)
	{
		if (e.targetControllable == Spatializer::getInstance()->textureOpacity) needsRepaint = true;
	}
}

void SpatializerPanel::newMessage(const SpatializerEvent & e)
{
	if (e.type == SpatializerEvent::LAYOUT_CHANGED)
	{
		setCurrentLayoutView(Spatializer::getInstance()->currentLayout);
	}
}

void SpatializerPanel::timerCallback()
{
	if (videoBlock == nullptr || !videoBlock->inputIsLive->boolValue()) return;
	if (needsRepaint) repaint();
}



// SPAT LAYOUT VIEW


SpatLayoutView::SpatLayoutView(Spatializer * spat, SpatLayout * layout) :
	ManagerShapeShifterUI(layout->niceName, &layout->spatItemManager, false),
	spat(spat),
	layout(layout)
{
	
	transparentBG = true;
	addExistingItems();
}

SpatLayoutView::~SpatLayoutView()
{
}


SpatItemUI * SpatLayoutView::createUIForItem(SpatItem * i)
{
	return new SpatItemUI(i, spat, this);
}


void SpatLayoutView::resized()
{
	for (auto &i : itemsUI)
	{
		i->updateBounds();
	}
}


Point<float> SpatLayoutView::getPositionForRelative(Point<float> relPos)
{
	return Point<float>(relPos.x * getWidth(), relPos.y * getHeight());
}

Point<float> SpatLayoutView::getRelativeForPosition(Point<float> absolutePos)
{
	return Point<float>(absolutePos.x / getWidth(), absolutePos.y / getHeight());
}
