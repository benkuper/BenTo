/*
  ==============================================================================

	LightBlockLibraryUI.cpp
	Created: 10 Apr 2018 10:58:08pm
	Author:  Ben

  ==============================================================================
*/

#include "LightBlock/LightBlockIncludes.h"

LightBlockModelLibraryUI::LightBlockModelLibraryUI(const String& contentName, LightBlockModelLibrary* library) :
	ShapeShifterContentComponent(contentName),
	library(library),
	patternGroupUI(&library->patternBlocks),
	sequenceBlocksManagerUI("Sequences", &library->sequenceBlocks),
	nodeBlocksManagerUI("Nodes", &library->nodeBlocks),
	pictureBlocksManagerUI("Pictures", &library->pictureBlocks),
	streamingScriptBlocksManagerUI("Streaming Scripts", &library->streamingScriptBlocks),
	embeddedScriptBlocksManagerUI("Embedded Scripts (Experimental)", &library->embeddedScriptBlocks),
	videoManagerUI("Videos (Experimental)", &library->videoBlocks)
	//genericFilterGroupUI(&library->genericFilterBlocks)
{
	iconSizeUI.reset(library->iconSize->createSlider());
	addAndMakeVisible(iconSizeUI.get());

	viewport.setViewedComponent(&container, false);
	viewport.setScrollBarsShown(true, false);
	viewport.setScrollOnDragMode(Viewport::ScrollOnDragMode::never);
	viewport.setScrollBarThickness(10);
	addAndMakeVisible(viewport);

	patternGroupUI.setThumbSize(library->iconSize->intValue());
	sequenceBlocksManagerUI.setThumbSize(library->iconSize->intValue());
	nodeBlocksManagerUI.setThumbSize(library->iconSize->intValue());
	pictureBlocksManagerUI.setThumbSize(library->iconSize->intValue());
	streamingScriptBlocksManagerUI.setThumbSize(library->iconSize->intValue());
	embeddedScriptBlocksManagerUI.setThumbSize(library->iconSize->intValue());
	videoManagerUI.setThumbSize(library->iconSize->intValue());
	//genericFilterGroupUI.setThumbSize(library->iconSize->intValue());


	container.addAndMakeVisible(&patternGroupUI);
	container.addAndMakeVisible(&sequenceBlocksManagerUI);
	container.addAndMakeVisible(&pictureBlocksManagerUI);
	container.addAndMakeVisible(&nodeBlocksManagerUI);
	container.addAndMakeVisible(&streamingScriptBlocksManagerUI);
	container.addAndMakeVisible(&embeddedScriptBlocksManagerUI);
	container.addAndMakeVisible(&videoManagerUI);

	//container.addAndMakeVisible(&genericFilterGroupUI);



	sequenceBlocksManagerUI.addComponentListener(this);
	pictureBlocksManagerUI.addComponentListener(this);
	nodeBlocksManagerUI.addComponentListener(this);
	streamingScriptBlocksManagerUI.addComponentListener(this);
	embeddedScriptBlocksManagerUI.addComponentListener(this);
	videoManagerUI.addComponentListener(this);

	library->addAsyncCoalescedContainerListener(this);
}

LightBlockModelLibraryUI::~LightBlockModelLibraryUI()
{
	if (LightBlockModelLibrary::getInstanceWithoutCreating() != nullptr) library->removeAsyncContainerListener(this);
}

void LightBlockModelLibraryUI::paint(Graphics& g)
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

	if (patternGroupUI.getWidth() == 0) patternGroupUI.setBounds(r);
	patternGroupUI.setBounds(r.withHeight(patternGroupUI.getHeight()));
	r.translate(0, patternGroupUI.getHeight() + 10);

	sequenceBlocksManagerUI.setBounds(r.withHeight(sequenceBlocksManagerUI.getHeight()));
	r.translate(0, sequenceBlocksManagerUI.getHeight() + 10);


	pictureBlocksManagerUI.setBounds(r.withHeight(pictureBlocksManagerUI.getHeight()));
	r.translate(0, pictureBlocksManagerUI.getHeight() + 10);

	nodeBlocksManagerUI.setBounds(r.withHeight(nodeBlocksManagerUI.getHeight()));
	r.translate(0, nodeBlocksManagerUI.getHeight() + 10);

	streamingScriptBlocksManagerUI.setBounds(r.withHeight(streamingScriptBlocksManagerUI.getHeight()));
	r.translate(0, streamingScriptBlocksManagerUI.getHeight() + 10);

	embeddedScriptBlocksManagerUI.setBounds(r.withHeight(embeddedScriptBlocksManagerUI.getHeight()));
	r.translate(0, embeddedScriptBlocksManagerUI.getHeight() + 10);

	videoManagerUI.setBounds(r.withHeight(videoManagerUI.getHeight()));
	r.translate(0, videoManagerUI.getHeight() + 10);

	/*r.translate(0, 10);
	//if (genericFilterGroupUI.getWidth() == 0) genericFilterGroupUI.setBounds(r);
	//genericFilterGroupUI.setBounds(r.withHeight(genericFilterGroupUI.getHeight()));*/
	//r.translate(0, genericFilterGroupUI.getHeight() + 10);

	container.setSize(getWidth(), r.getBottom());
	viewport.setBounds(getLocalBounds().withTrimmedTop(cy));
}

void LightBlockModelLibraryUI::newMessage(const ContainerAsyncEvent& e)
{
	switch (e.type)
	{
	case ContainerAsyncEvent::ControllableFeedbackUpdate:
	{
		if (e.targetControllable == library->iconSize)
		{
			patternGroupUI.setThumbSize(library->iconSize->intValue());
			sequenceBlocksManagerUI.setThumbSize(library->iconSize->intValue());
			pictureBlocksManagerUI.setThumbSize(library->iconSize->intValue());
			nodeBlocksManagerUI.setThumbSize(library->iconSize->intValue());
			streamingScriptBlocksManagerUI.setThumbSize(library->iconSize->intValue());
			embeddedScriptBlocksManagerUI.setThumbSize(library->iconSize->intValue());
			videoManagerUI.setThumbSize(library->iconSize->intValue());
			//genericFilterGroupUI.setThumbSize(library->iconSize->intValue());

			resized();
		}
	}
	break;

	default:
		break;
	}
}

void LightBlockModelLibraryUI::componentMovedOrResized(Component&, bool, bool)
{
	resized();
}
