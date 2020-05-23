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
	timelineBlocksManagerUI("Timelines", &library->timelineBlocks),
	genericFilterGroupUI(&library->genericFilterBlocks)
{
	iconSizeUI.reset(library->iconSize->createSlider());
	addAndMakeVisible(iconSizeUI.get());

	viewport.setViewedComponent(&container, false);
	viewport.setScrollBarsShown(true, false);
	viewport.setScrollOnDragEnabled(false);
	viewport.setScrollBarThickness(10);
	addAndMakeVisible(viewport);

	genericGroupUI.setThumbSize(library->iconSize->intValue());
	liveFeedGroupUI.setThumbSize(library->iconSize->intValue());
	pictureBlocksManagerUI.setThumbSize(library->iconSize->intValue());
	nodeBlocksManagerUI.setThumbSize(library->iconSize->intValue());
	scriptBlocksManagerUI.setThumbSize(library->iconSize->intValue());
	timelineBlocksManagerUI.setThumbSize(library->iconSize->intValue());
	genericFilterGroupUI.setThumbSize(library->iconSize->intValue());


	container.addAndMakeVisible(&genericGroupUI);
	container.addAndMakeVisible(&liveFeedGroupUI);

	container.addAndMakeVisible(&pictureBlocksManagerUI);
	container.addAndMakeVisible(&nodeBlocksManagerUI);
	container.addAndMakeVisible(&scriptBlocksManagerUI);
	container.addAndMakeVisible(&timelineBlocksManagerUI);

	container.addAndMakeVisible(&genericFilterGroupUI);


	pictureBlocksManagerUI.addComponentListener(this);
	nodeBlocksManagerUI.addComponentListener(this);
	scriptBlocksManagerUI.addComponentListener(this);
	timelineBlocksManagerUI.addComponentListener(this);

	library->addAsyncCoalescedContainerListener(this);
}

LightBlockModelLibraryUI::~LightBlockModelLibraryUI()
{
	if (LightBlockModelLibrary::getInstanceWithoutCreating() != nullptr) library->removeAsyncContainerListener(this);
}

void LightBlockModelLibraryUI::paint(Graphics & g)
{
}

void LightBlockModelLibraryUI::resized()
{
	Rectangle<int> r = getLocalBounds();
	Rectangle<int> hr = r.removeFromTop(20);

	iconSizeUI->setBounds(hr.removeFromRight(100).reduced(2));

	r.removeFromTop(2);
	int cy = r.getY();

	r.setY(0);
	r = r.withTrimmedRight(10);

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

	timelineBlocksManagerUI.setBounds(r.withHeight(timelineBlocksManagerUI.getHeight()));
	r.translate(0, timelineBlocksManagerUI.getHeight() + 10);

	r.translate(0, 10);
	if (genericFilterGroupUI.getWidth() == 0) genericFilterGroupUI.setBounds(r);
	genericFilterGroupUI.setBounds(r.withHeight(genericFilterGroupUI.getHeight()));
	r.translate(0, genericFilterGroupUI.getHeight() + 10);

	container.setSize(getWidth(), r.getBottom());
	viewport.setBounds(getLocalBounds().withTrimmedTop(cy));
}

void LightBlockModelLibraryUI::newMessage(const ContainerAsyncEvent & e)
{
	switch (e.type)
	{
	case ContainerAsyncEvent::ControllableFeedbackUpdate:
	{
		if (e.targetControllable == library->iconSize)
		{
			genericGroupUI.setThumbSize(library->iconSize->intValue());
			liveFeedGroupUI.setThumbSize(library->iconSize->intValue());
			pictureBlocksManagerUI.setThumbSize(library->iconSize->intValue());
			nodeBlocksManagerUI.setThumbSize(library->iconSize->intValue());
			scriptBlocksManagerUI.setThumbSize(library->iconSize->intValue());
			timelineBlocksManagerUI.setThumbSize(library->iconSize->intValue());
			genericFilterGroupUI.setThumbSize(library->iconSize->intValue());

			resized();
		}
	}
	break;

	default:
		break;
	}
}

void LightBlockModelLibraryUI::componentMovedOrResized(Component &, bool, bool)
{
	resized();
}
