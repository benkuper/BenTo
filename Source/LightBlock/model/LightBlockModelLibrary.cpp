/*
  ==============================================================================

    LightBlockModelLibrary.cpp
    Created: 10 Apr 2018 10:57:18pm
    Author:  Ben

  ==============================================================================
*/

#include "LightBlockModelLibrary.h"
#include "blocks/timeline/TimelineBlock.h"
#include "blocks/pattern/PatternBlock.h"
#include "blocks/video/VideoBlock.h"
#include "blocks/dmx/DMXBlock.h"

juce_ImplementSingleton(LightBlockModelLibrary)


LightBlockModelLibrary::LightBlockModelLibrary() :
	ControllableContainer("Block Library"),
	genericBlocks("Generic"),
	liveFeedBlocks("Live Feed")
{
	//patterns
	solidColorBlock = new PatternBlock("Solid Color");
	rainbowBlock = new PatternBlock("Solid Color");
	strobeBlock = new PatternBlock("Strobe");
	noiseBlock = new PatternBlock("Noise");

	timelineBlock = new TimelineBlock();
	videoBlock = new VideoBlock();
	dmxBlock = new DMXBlock();

	genericBlocks.addChildControllableContainer(timelineBlock);
	genericBlocks.addChildControllableContainer(solidColorBlock);
	genericBlocks.addChildControllableContainer(rainbowBlock);
	genericBlocks.addChildControllableContainer(strobeBlock);
	genericBlocks.addChildControllableContainer(noiseBlock);
	addChildControllableContainer(&genericBlocks);

	liveFeedBlocks.addChildControllableContainer(videoBlock);
	liveFeedBlocks.addChildControllableContainer(dmxBlock);
	addChildControllableContainer(&liveFeedBlocks);

	addChildControllableContainer(&userBlocks);
}

LightBlockModelLibrary::~LightBlockModelLibrary()
{
}
