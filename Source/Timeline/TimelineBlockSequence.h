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
class Prop;

class TimelineBlockSequence :
	public Sequence,
	public SequenceLayerManager::Listener
{
public:
	TimelineBlockSequence();
	~TimelineBlockSequence();

	SequenceLayerFactory layerFactory;
	Array<Colour> getColors(Prop * p, double time, var params);
	Array<LightBlockLayer *> getLayersForProp(Prop * p);

	void itemAdded(SequenceLayer * s) override;

	void onControllableFeedbackUpdateInternal(ControllableContainer * cc, Controllable * c) override;
};