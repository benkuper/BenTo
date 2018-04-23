/*
  ==============================================================================

    LightBlockLibraryUI.cpp
    Created: 10 Apr 2018 10:58:08pm
    Author:  Ben

  ==============================================================================
*/

#include "LightBlockModelLibraryUI.h"


LightBlockModelLibraryUI::LightBlockModelLibraryUI(const String &contentName, LightBlockModelLibrary * library) :
	ShapeShifterContentComponent(contentName),
	library(library),
	genericGroupUI(&library->genericBlocks),
	liveFeedGroupUI(&library->liveFeedBlocks),
	pictureBlocksManagerUI("Pictures", &library->pictureBlocks),
	nodeBlocksManagerUI("Nodes", &library->nodeBlocks),
	scriptBlocksManagerUI("Scripts", &library->scriptBlocks),
	timelineBlocksManagerUI("Timelines",&library->timelineBlocks)
{
	viewport.setViewedComponent(&container, false);
	viewport.setScrollBarsShown(true, false);
	viewport.setScrollOnDragEnabled(false);
	viewport.setScrollBarThickness(10);
	addAndMakeVisible(viewport);


	container.addAndMakeVisible(&genericGroupUI);
	container.addAndMakeVisible(&liveFeedGroupUI);

	container.addAndMakeVisible(&pictureBlocksManagerUI);
	container.addAndMakeVisible(&nodeBlocksManagerUI);
	container.addAndMakeVisible(&scriptBlocksManagerUI);
	container.addAndMakeVisible(&timelineBlocksManagerUI);

	pictureBlocksManagerUI.addComponentListener(this);
	nodeBlocksManagerUI.addComponentListener(this);
	scriptBlocksManagerUI.addComponentListener(this);
	timelineBlocksManagerUI.addComponentListener(this);
}

LightBlockModelLibraryUI::~LightBlockModelLibraryUI()
{
}

void LightBlockModelLibraryUI::paint(Graphics & g)
{
}

void LightBlockModelLibraryUI::resized()
{
	Rectangle<int> r = getLocalBounds().withTrimmedRight(10);

	if (genericGroupUI.getWidth() == 0) genericGroupUI.setBounds(r);
	genericGroupUI.setBounds(r.withHeight(genericGroupUI.getHeight()));
	r.translate(0, genericGroupUI.getHeight() + 10);

	if (liveFeedGroupUI.getWidth() == 0) liveFeedGroupUI.setBounds(r);
	liveFeedGroupUI.setBounds(r.withHeight(liveFeedGroupUI.getHeight()));
	r.translate(0, liveFeedGroupUI.getHeight() + 10);

	pictureBlocksManagerUI.setBounds(r.withHeight(pictureBlocksManagerUI.getHeight()));
	r.translate(0, pictureBlocksManagerUI.getHeight() + 10);

	nodeBlocksManagerUI.setBounds(r.withHeight(nodeBlocksManagerUI.getHeight()));
	r.translate(0, nodeBlocksManagerUI.getHeight() + 10);

	scriptBlocksManagerUI.setBounds(r.withHeight(scriptBlocksManagerUI.getHeight()));
	r.translate(0, scriptBlocksManagerUI.getHeight() + 10);

	r.setHeight(timelineBlocksManagerUI.getHeight());
	timelineBlocksManagerUI.setBounds(r);

	container.setSize(getWidth(), r.getBottom());
	viewport.setSize(getWidth(),getHeight());
}

void LightBlockModelLibraryUI::componentMovedOrResized(Component &, bool, bool)
{
	resized();
}
