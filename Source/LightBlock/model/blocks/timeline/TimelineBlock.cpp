/*
  ==============================================================================

    TimelineBlock.cpp
    Created: 10 Apr 2018 6:59:02pm
    Author:  Ben

  ==============================================================================
*/

#include "TimelineBlock.h"
#include "Timeline/layers/Block/LightBlockLayer.h"
#include "ui/TimelineBlockUI.h"

TimelineBlock::TimelineBlock(var params) :
	LightBlockModel(getTypeString(), params)
{
	sequence.addSequenceListener(this);
	addChildControllableContainer(&sequence);
}

TimelineBlock::~TimelineBlock()
{
}

var TimelineBlock::getJSONData()
{
	var data = LightBlockModel::getJSONData();
	data.getDynamicObject()->setProperty("sequence", sequence.getJSONData());
	return data;
}

void TimelineBlock::loadJSONDataInternal(var data)
{
	LightBlockModel::loadJSONDataInternal(data);
	sequence.loadJSONData(data.getProperty("sequence", var()));
}

LightBlockModelUI * TimelineBlock::createUI()
{
	return new TimelineBlockUI(this);
}

Array<Colour> TimelineBlock::getColors(Prop * p, double time, var params)
{
	return sequence.getColors(p, time, params);

}

void TimelineBlock::sequenceCurrentTimeChanged(Sequence *, float, bool)
{
	//updateColorsForBlock(block);
}

