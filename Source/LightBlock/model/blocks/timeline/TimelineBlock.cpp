/*
  ==============================================================================

    TimelineBlock.cpp
    Created: 10 Apr 2018 6:59:02pm
    Author:  Ben

  ==============================================================================
*/

#include "LightBlock/LightBlockIncludes.h"
#include "Timeline/TimelineIncludes.h"
#include "BentoEngine.h"

TimelineBlock::TimelineBlock(var params) :
	LightBlockModel(getTypeString(), params)
{
	itemDataType = getTypeString();

	bakeToProps = addTrigger("Bake to props", "");

	autoSetPropEnabled = addBoolParameter("Auto Set enabled", "If checked, this will automatically enable / disable prop based on whether they are filtered by the sequence", false);

	sequence.reset(new TimelineBlockSequence());
	sequence->addSequenceListener(this);
	addChildControllableContainer(sequence.get());
}

TimelineBlock::~TimelineBlock()
{
}

var TimelineBlock::getJSONData()
{
	var data = LightBlockModel::getJSONData();
	data.getDynamicObject()->setProperty("sequence", sequence->getJSONData());
	return data;
}

void TimelineBlock::loadJSONDataInternal(var data)
{
	LightBlockModel::loadJSONDataInternal(data);
	sequence->loadJSONData(data.getProperty("sequence", var()));
}

LightBlockModelUI * TimelineBlock::createUI()
{
	return new TimelineBlockUI(this);
}


Array<Colour> TimelineBlock::getColors(Prop * p, double time, var params)
{
	return sequence->getColors(p, time, params);

}

BakeData TimelineBlock::getBakeDataForProp(Prop* p)
{
	var metaData = new DynamicObject();

	int groupID = -1;
	int localID = -1;
	
	if (sequence->currentIdentityGroup != nullptr)
	{
		groupID = sequence->currentIdentityGroup->getClusterIDForProp(p);
		PropCluster* c = sequence->currentIdentityGroup->getClusterForProp(p, localID);
		if (c != nullptr)
		{
			metaData.getDynamicObject()->setProperty("groupColor", c->color->value);
		}
	}

	metaData.getDynamicObject()->setProperty("id", localID);
	metaData.getDynamicObject()->setProperty("group", groupID);
	metaData.getDynamicObject()->setProperty("scripts", sequence->getWasmTimingsDataForProp(p));

	return BakeData(shortName, 0, sequence->totalTime->floatValue(), sequence->fps->intValue(), metaData);
}

void TimelineBlock::sequenceCurrentTimeChanged(Sequence *, float prevTime, bool)
{
	if (sequence->isSeeking)
	{
		providerListeners.call(&ProviderListener::providerBakeControlUpdate, SEEK, sequence->currentTime->floatValue());
	}

	OSCMessage msg("/" + shortName + "/currentTime");
	msg.addFloat32(sequence->currentTime->floatValue());
	BentoEngine* be = (BentoEngine*)Engine::mainEngine;
	be->globalSender.sendToIPAddress(be->remoteHost->stringValue(), be->remotePort->intValue(), msg);
	//updateColorsForBlock(block);
}

void TimelineBlock::sequencePlayStateChanged(Sequence * s)
{
	providerListeners.call(&ProviderListener::providerBakeControlUpdate, sequence->isPlaying->boolValue() ? PLAY : PAUSE, sequence->currentTime->floatValue());
}

void TimelineBlock::sequenceEditingStateChanged(Sequence * s)
{
	setBeingEdited(sequence->isBeingEdited);
}

void TimelineBlock::sequenceLooped(Sequence * s)
{
	providerListeners.call(&ProviderListener::providerBakeControlUpdate, PLAY, 0);
}

void TimelineBlock::onContainerTriggerTriggered(Trigger* t)
{
	LightBlockModel::onContainerTriggerTriggered(t);

	if (t == bakeToProps)
	{
		for (auto& p : PropManager::getInstance()->items) p->initBaking(this, Prop::AfterBakeAction::UPLOAD);
	}
}

void TimelineBlock::onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c)
{
	LightBlockModel::onControllableFeedbackUpdateInternal(cc, c);
	if (c == sequence->identityMode)
	{
		providerListeners.call(&ProviderListener::providerBakeControlUpdate, SHOW_ID, sequence->identityMode->boolValue());
	} 
}

