/*
  ==============================================================================

    SequenceBlock.cpp
    Created: 10 Apr 2018 6:59:02pm
    Author:  Ben

  ==============================================================================
*/

#include "LightBlock/LightBlockIncludes.h"
#include "Sequence/SequenceIncludes.h"
#include "BentoEngine.h"

BentoSequenceBlock::BentoSequenceBlock(var params) :
	LightBlockModel("Sequence", params)
{
	itemDataType = getTypeString();

	uploadPlaybackToProps = addTrigger("Upload Playback", "");

	autoSetPropEnabled = addBoolParameter("Auto Set enabled", "If checked, this will automatically enable / disable prop based on whether they are filtered by the sequence", false);

	sequence.reset(new SequenceBlockSequence());
	sequence->userCanRemove = false;
	sequence->userCanDuplicate = false;
	sequence->setCanBeDisabled(false);
	sequence->addSequenceListener(this);
	addChildControllableContainer(sequence.get());
}

BentoSequenceBlock::~BentoSequenceBlock()
{
}

var BentoSequenceBlock::getJSONData(bool includeNonOverriden)
{
	var data = LightBlockModel::getJSONData(includeNonOverriden);
	data.getDynamicObject()->setProperty("sequence", sequence->getJSONData());
	return data;
}

void BentoSequenceBlock::loadJSONDataInternal(var data)
{
	LightBlockModel::loadJSONDataInternal(data);
	sequence->loadJSONData(data.getProperty("sequence", var()));
}

LightBlockModelUI* BentoSequenceBlock::createUI()
{
	return new BentoSequenceBlockUI(this);
}


Array<Colour> BentoSequenceBlock::getColors(Prop* p, double time, var params)
{
	return sequence->getColors(p, time, params);

}

PlaybackData BentoSequenceBlock::getPlaybackDataForProp(Prop* p)
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

	return PlaybackData(shortName, 0, sequence->totalTime->floatValue(), sequence->fps->intValue(), metaData);
}

void BentoSequenceBlock::sequenceCurrentTimeChanged(Sequence*, float prevTime, bool)
{
	if (sequence->isSeeking)
	{
		providerListeners.call(&ColorProviderListener::providerPlaybackControlUpdate, SEEK, sequence->currentTime->floatValue());
	}

	//OSCMessage msg("/" + shortName + "/currentTime");
	//msg.addFloat32(sequence->currentTime->floatValue());
	//BentoEngine* be = (BentoEngine*)Engine::mainEngine;
	//be->globalSender.sendToIPAddress(be->remoteHost->stringValue(), be->remotePort->intValue(), msg);
	//updateColorsForBlock(block);
}

void BentoSequenceBlock::sequencePlayStateChanged(Sequence* s)
{
	providerListeners.call(&ColorProviderListener::providerPlaybackControlUpdate, sequence->isPlaying->boolValue() ? PLAY : PAUSE, sequence->currentTime->floatValue());
}

void BentoSequenceBlock::sequenceEditingStateChanged(Sequence* s)
{
	setBeingEdited(sequence->isBeingEdited);
}

void BentoSequenceBlock::sequenceLooped(Sequence* s)
{
	providerListeners.call(&ColorProviderListener::providerPlaybackControlUpdate, PLAY, 0);
}

void BentoSequenceBlock::onContainerTriggerTriggered(Trigger* t)
{
	LightBlockModel::onContainerTriggerTriggered(t);

	if (t == uploadPlaybackToProps)
	{
		for (auto& p : PropManager::getInstance()->items) p->initGeneratePlayback(this, Prop::AfterPlaybackGenAction::UPLOAD);
	}
}

void BentoSequenceBlock::onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c)
{
	LightBlockModel::onControllableFeedbackUpdateInternal(cc, c);
	if (c == sequence->identityMode)
	{
		providerListeners.call(&ColorProviderListener::providerPlaybackControlUpdate, SHOW_ID, sequence->identityMode->boolValue());
	} 
}

