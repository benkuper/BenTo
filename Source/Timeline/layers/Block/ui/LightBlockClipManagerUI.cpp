/*
  ==============================================================================

    LightBlockClipManagerUI.cpp
    Created: 17 Apr 2018 7:20:49pm
    Author:  Ben

  ==============================================================================
*/

#include "LightBlockClipManagerUI.h"
#include "LightBlockLayerTimeline.h"
#include "LightBlock/model/ui/LightBlockModelUI.h"


LightBlockClipManagerUI::LightBlockClipManagerUI(LightBlockLayerTimeline * _timeline, LightBlockClipManager * manager) :
	LayerBlockManagerUI(_timeline, manager),
	clipManager(manager),
	timeline(_timeline)
{
	addExistingItems();
}

LightBlockClipManagerUI::~LightBlockClipManagerUI()
{
}

LayerBlockUI * LightBlockClipManagerUI::createUIForItem(LayerBlock * item)
{
	return new LightBlockClipUI((LightBlockClip *)item);
}

bool LightBlockClipManagerUI::isInterestedInDragSource(const SourceDetails & source)
{
	return source.description == "LightBlockModel";
}

void LightBlockClipManagerUI::itemDragEnter(const SourceDetails & source)
{
}

void LightBlockClipManagerUI::itemDragExit(const SourceDetails & source)
{
}

void LightBlockClipManagerUI::itemDragMove(const SourceDetails & source)
{
}

void LightBlockClipManagerUI::itemDropped(const SourceDetails & source)
{
	LightBlockModelUI * modelUI = dynamic_cast<LightBlockModelUI *>(source.sourceComponent.get());
	LightBlockClip * clip = (LightBlockClip *)manager->addBlockAt(timeline->getTimeForX(source.localPosition.x));
	if (modelUI == nullptr || clip == nullptr) return;

	clip->activeProvider->setValueFromTarget(modelUI->item);
}
