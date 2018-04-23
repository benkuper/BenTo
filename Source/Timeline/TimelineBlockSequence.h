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
	Array<Colour> getColors(int id, int resolution, float time, var params);
	LightBlockLayer * getLayerForID(int id);

	void onControllableFeedbackUpdateInternal(ControllableContainer * cc, Controllable * c) override;
};