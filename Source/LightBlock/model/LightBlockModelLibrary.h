/*
  ==============================================================================

    LightBlockModelLibrary.h
    Created: 10 Apr 2018 10:57:18pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "UserLightBlockModelManager.h"

class LightBlockModelLibrary :
	public ControllableContainer
{
public:
	juce_DeclareSingleton(LightBlockModelLibrary, true)
	LightBlockModelLibrary();
	~LightBlockModelLibrary();

	ControllableContainer genericBlocks;
	ControllableContainer liveFeedBlocks;
	UserLightBlockModelManager userBlocks;	

	//Generic
	ScopedPointer<LightBlockModel> timelineBlock;
	//Patterns
	ScopedPointer<LightBlockModel> solidColorBlock;
	ScopedPointer<LightBlockModel> rainbowBlock;
	ScopedPointer<LightBlockModel> strobeBlock;
	ScopedPointer<LightBlockModel> noiseBlock;

	//Live Feed
	ScopedPointer<LightBlockModel> videoBlock;
	ScopedPointer<LightBlockModel> dmxBlock;
};