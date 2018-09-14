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
	public Sequence,
	public SequenceLayerManager::Listener
{
public:
	TimelineBlockSequence();
	~TimelineBlockSequence();

	SequenceLayerFactory layerFactory;
	Array<Colour> getColors(int id, int resolution, double time, var params);
	Array<LightBlockLayer *> getLayersForID(int id);

	void itemAdded(SequenceLayer * s) override;

	void onControllableFeedbackUpdateInternal(ControllableContainer * cc, Controllable * c) override;
};