/*
  ==============================================================================

	SpatializerPanel.cpp
	Created: 23 Apr 2018 9:42:51pm
	Author:  Ben

  ==============================================================================
*/

#include "SpatializerPanel.h"
#include "LightBlock/model/LightBlockModelLibrary.h"
#include "LightBlock/model/blocks/video/VideoBlock.h"

SpatializerPanel::SpatializerPanel(const String & name) :
	ShapeShifterContentComponent(name),
	needsRepaint(true)
{
	videoBlock = dynamic_cast<VideoBlock *>(LightBlockModelLibrary::getInstance()->videoBlock.get());

	setCurrentLayoutView(videoBlock->spat.currentLayout);
	startTimerHz(30); //repaint at 30hz

	videoBlock->spat.addAsyncSpatListener(this);
}


SpatializerPanel::~SpatializerPanel()
{
	if (videoBlock != nullptr) videoBlock->spat.removeAsyncSpatListener(this);
}


void SpatializerPanel::setCurrentLayoutView(SpatLayout * layout)
{
	if(currentLayoutView != nullptr && layout == currentLayoutView->layout) return;
	if (currentLayoutView != nullptr)
	{
		removeChildComponent(currentLayoutView);
		currentLayoutView = nullptr;
	}
	
	if(layout != nullptr)
	{
		currentLayoutView = new SpatLayoutView(&videoBlock->spat, layout);
		addAndMakeVisible(currentLayoutView);
		resized();
	}
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

	if (videoBlock != nullptr && videoBlock->inputIsLive->boolValue())
	{
		g.setColour(Colours::white.withAlpha(videoBlock->spat.textureOpacity->floatValue()));
		g.drawImage(videoBlock->receiver->getImage(), getLocalBounds().toFloat());
	}
}

void SpatializerPanel::resized()
{
	if (currentLayoutView != nullptr)
	{
		currentLayoutView->setBounds(getLocalBounds());
	}
}

void SpatializerPanel::newMessage(const ContainerAsyncEvent & e)
{
	if (e.type == ContainerAsyncEvent::ControllableFeedbackUpdate)
	{
		if (e.targetControllable == videoBlock->spat.textureOpacity) needsRepaint = true;
	}
}

void SpatializerPanel::newMessage(const SpatializerEvent & e)
{
	if (e.type == SpatializerEvent::LAYOUT_CHANGED)
	{
		setCurrentLayoutView(videoBlock->spat.currentLayout);
	}
}

void SpatializerPanel::timerCallback()
{
	if (videoBlock == nullptr || !videoBlock->inputIsLive->boolValue()) return;
	if (needsRepaint) repaint();
}



// SPAT LAYOUT VIEW


SpatLayoutView::SpatLayoutView(Spatializer * spat, SpatLayout * layout) :
	BaseManagerShapeShifterUI(layout->niceName, &layout->spatItemManager, false),
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
