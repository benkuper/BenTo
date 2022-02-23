/*
  ==============================================================================

    LightBlockLibraryUI.cpp
    Created: 10 Apr 2018 10:58:08pm
    Author:  Ben

  ==============================================================================
*/

LightBlockModelLibraryUI::LightBlockModelLibraryUI(const String &contentName, LightBlockModelLibrary * library) :
	ShapeShifterContentComponent(contentName),
	library(library),
	genericGroupUI(&library->genericBlocks),
	liveFeedManagerUI("Live Feed", &library->liveFeedBlocks),
	videoManagerUI("Video", &library->videoBlocks),
	pictureBlocksManagerUI("Pictures", &library->pictureBlocks),
	nodeBlocksManagerUI("Nodes", &library->nodeBlocks),
	scriptBlocksManagerUI("Scripts", &library->scriptBlocks),
	wasmBlocksManagerUI("Wasm", &library->wasmBlocks),
	timelineBlocksManagerUI("Timelines", &library->timelineBlocks),
	genericFilterGroupUI(&library->genericFilterBlocks)
{
	iconSizeUI.reset(library->iconSize->createSlider());
	addAndMakeVisible(iconSizeUI.get());

	viewport.setViewedComponent(&container, false);
	viewport.setScrollBarsShown(true, false);
	viewport.setScrollOnDragMode(Viewport::ScrollOnDragMode::never);
	viewport.setScrollBarThickness(10);
	addAndMakeVisible(viewport);

	genericGroupUI.setThumbSize(library->iconSize->intValue());
	liveFeedManagerUI.setThumbSize(library->iconSize->intValue());
	videoManagerUI.setThumbSize(library->iconSize->intValue());
	pictureBlocksManagerUI.setThumbSize(library->iconSize->intValue());
	nodeBlocksManagerUI.setThumbSize(library->iconSize->intValue());
	scriptBlocksManagerUI.setThumbSize(library->iconSize->intValue());
	wasmBlocksManagerUI.setThumbSize(library->iconSize->intValue());
	timelineBlocksManagerUI.setThumbSize(library->iconSize->intValue());
	genericFilterGroupUI.setThumbSize(library->iconSize->intValue());


	container.addAndMakeVisible(&genericGroupUI);

	container.addAndMakeVisible(&liveFeedManagerUI);
	container.addAndMakeVisible(&videoManagerUI);
	container.addAndMakeVisible(&pictureBlocksManagerUI);
	container.addAndMakeVisible(&nodeBlocksManagerUI);
	container.addAndMakeVisible(&scriptBlocksManagerUI);
	container.addAndMakeVisible(&wasmBlocksManagerUI);
	container.addAndMakeVisible(&timelineBlocksManagerUI);

	container.addAndMakeVisible(&genericFilterGroupUI);


	liveFeedManagerUI.addComponentListener(this);
	videoManagerUI.addComponentListener(this);
	pictureBlocksManagerUI.addComponentListener(this);
	nodeBlocksManagerUI.addComponentListener(this);
	scriptBlocksManagerUI.addComponentListener(this);
	wasmBlocksManagerUI.addComponentListener(this);
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

	liveFeedManagerUI.setBounds(r.withHeight(liveFeedManagerUI.getHeight()));
	r.translate(0, liveFeedManagerUI.getHeight() + 10);
	
	videoManagerUI.setBounds(r.withHeight(videoManagerUI.getHeight()));
	r.translate(0, videoManagerUI.getHeight() + 10);
	
	pictureBlocksManagerUI.setBounds(r.withHeight(pictureBlocksManagerUI.getHeight()));
	r.translate(0, pictureBlocksManagerUI.getHeight() + 10);

	nodeBlocksManagerUI.setBounds(r.withHeight(nodeBlocksManagerUI.getHeight()));
	r.translate(0, nodeBlocksManagerUI.getHeight() + 10);

	scriptBlocksManagerUI.setBounds(r.withHeight(scriptBlocksManagerUI.getHeight()));
	r.translate(0, scriptBlocksManagerUI.getHeight() + 10);
	
	wasmBlocksManagerUI.setBounds(r.withHeight(wasmBlocksManagerUI.getHeight()));
	r.translate(0, wasmBlocksManagerUI.getHeight() + 10);

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
			liveFeedManagerUI.setThumbSize(library->iconSize->intValue());
			videoManagerUI.setThumbSize(library->iconSize->intValue());
			pictureBlocksManagerUI.setThumbSize(library->iconSize->intValue());
			nodeBlocksManagerUI.setThumbSize(library->iconSize->intValue());
			scriptBlocksManagerUI.setThumbSize(library->iconSize->intValue());
			wasmBlocksManagerUI.setThumbSize(library->iconSize->intValue());
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
