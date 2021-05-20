/*
  ==============================================================================

    TimelineBlock.h
    Created: 10 Apr 2018 6:59:02pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

class TimelineBlockSequence;
class LightBlock;

class TimelineBlock :
	public LightBlockModel,
	public Sequence::SequenceListener
{
public:
	TimelineBlock(var params = var());
	~TimelineBlock();

	Trigger* bakeToProps;

	std::unique_ptr<TimelineBlockSequence> sequence;

	Array<Colour> getColors(Prop * p, double time, var params) override;

	BakeData getBakeDataForProp(Prop * p) override;


	void sequenceCurrentTimeChanged(Sequence *, float /*prevTime*/, bool /*evaluateSkippedData*/) override;
	void sequencePlayStateChanged(Sequence * s) override;
	void sequenceEditingStateChanged(Sequence *s) override;

	void sequenceLooped(Sequence * s) override;

	void onContainerTriggerTriggered(Trigger* t) override;
	void onControllableFeedbackUpdateInternal(ControllableContainer*, Controllable* c) override;

	var getJSONData() override;
	void loadJSONDataInternal(var data) override;


	String getTypeString() const override { return "Timeline"; }
	LightBlockModelUI * createUI() override;
	static TimelineBlock * create(var params) { return new TimelineBlock(params); }
};