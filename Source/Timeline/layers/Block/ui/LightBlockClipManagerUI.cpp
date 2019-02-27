/*
  ==============================================================================

    LightBlockClipManagerUI.cpp
    Created: 17 Apr 2018 7:20:49pm
    Author:  Ben

  ==============================================================================
*/

#include "LightBlockClipManagerUI.h"
#include "LightBlockLayerTimeline.h"

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
