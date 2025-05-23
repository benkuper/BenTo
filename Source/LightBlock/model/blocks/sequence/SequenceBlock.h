/*
  ==============================================================================

    SequenceBlock.h
    Created: 10 Apr 2018 6:59:02pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

class SequenceBlockSequence;
class LightBlock;

class BentoSequenceBlock :
	public LightBlockModel,
	public Sequence::SequenceListener
{
public:
	BentoSequenceBlock(var params = var());
	~BentoSequenceBlock();

	Trigger* uploadPlaybackToProps;
	BoolParameter* autoSetPropEnabled;

	std::unique_ptr<SequenceBlockSequence> sequence;

	Array<Colour> getColors(Prop * p, double time, var params) override;

	PlaybackData getPlaybackDataForProp(Prop * p) override;


	void sequenceCurrentTimeChanged(Sequence *, float /*prevTime*/, bool /*evaluateSkippedData*/) override;
	void sequencePlayStateChanged(Sequence * s) override;
	void sequenceEditingStateChanged(Sequence *s) override;

	void sequenceLooped(Sequence * s) override;

	void onContainerTriggerTriggered(Trigger* t) override;
	void onControllableFeedbackUpdateInternal(ControllableContainer*, Controllable* c) override;

	var getJSONData(bool includeNonOverriden = false) override;
	void loadJSONDataInternal(var data) override;

	LightBlockModelUI* createUI() override;
	DECLARE_TYPE("SequenceBlock")

};