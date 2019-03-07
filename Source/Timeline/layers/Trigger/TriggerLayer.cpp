/*
  ==============================================================================

    TriggerLayer.cpp
    Created: 17 Nov 2016 7:59:54pm
    Author:  Ben Kuper

  ==============================================================================
*/

#include "TriggerLayer.h"
#include "ui/TriggerLayerPanel.h"
#include "ui/TriggerLayerTimeline.h"

TriggerLayer::TriggerLayer(Sequence * _sequence, var params) :
	SequenceLayer(_sequence, "New Trigger Layer")
{
	helpID = "TriggerLayer";
	color->setColor(ACTION_COLOR);

	lockAll = addTrigger("Lock All", "Lock all existing triggers in this layer");
	unlockAll = addTrigger("Unlock All", "Unlock all existing triggers in this layer");
}

TriggerLayer::~TriggerLayer()
{
}


void TriggerLayer::setManager(TimeTriggerManager * _ttm)
{
	ttm = _ttm;
	if (ttm != nullptr) addChildControllableContainer(ttm);
}

bool TriggerLayer::paste()
{
	if (ttm == nullptr) return false;
	TimeTrigger * p = ttm->addItemFromClipboard(false);
	if (p == nullptr) return BaseItem::paste();
	return true;
}

void TriggerLayer::onContainerTriggerTriggered(Trigger * t)
{
	SequenceLayer::onContainerTriggerTriggered(t);
	
	if (ttm == nullptr) return;

	if (t == lockAll) for (auto &i : ttm->items) i->isLocked->setValue(true);
	else if (t == unlockAll) for (auto &i : ttm->items) i->isLocked->setValue(false);
}

var TriggerLayer::getJSONData()
{
	var data = SequenceLayer::getJSONData();
	if(ttm != nullptr) data.getDynamicObject()->setProperty("triggerManager", ttm->getJSONData());
	return data;
}

void TriggerLayer::loadJSONDataInternal(var data)
{
	SequenceLayer::loadJSONDataInternal(data);
	if(ttm != nullptr) ttm->loadJSONData(data.getProperty("triggerManager", var()));
}

SequenceLayerPanel * TriggerLayer::getPanel()
{
	return new TriggerLayerPanel(this);
}

SequenceLayerTimeline * TriggerLayer::getTimelineUI()
{
	return new TriggerLayerTimeline(this);
}
