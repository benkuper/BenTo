/*
  ==============================================================================

    TimelineBlockSequence.h
    Created: 17 Apr 2018 8:30:13pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"

class LightBlock;
class LightBlockLayer;

class TimelineBlockSequence :
	public Sequence
{
public:
	TimelineBlockSequence();
	~TimelineBlockSequence();

	SequenceLayerFactory layerFactory;

	void updateColorsForBlock(LightBlock *b, var params = var());

	LightBlockLayer * getLayerForPropID(int id);
};