/*
  ==============================================================================

    TimelineBlock.h
    Created: 10 Apr 2018 6:59:02pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "../../LightBlockModel.h"
#include "Timeline/TimelineBlockSequence.h"

class LightBlock;

class TimelineBlock :
	public LightBlockModel,
	public Sequence::SequenceListener
{
public:
	TimelineBlock(var params = var());
	~TimelineBlock();

	TimelineBlockSequence sequence;

	void updateColorsForBlock(LightBlock *b, var params = var());

	void sequenceCurrentTimeChanged(Sequence *, float /*prevTime*/, bool /*evaluateSkippedData*/) override;

	var getJSONData() override;
	void loadJSONDataInternal(var data) override;

	String getTypeString() const override { return "Timeline"; }

	static TimelineBlock * create(var params) { return new TimelineBlock(params); }
};