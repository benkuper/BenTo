/*
  ==============================================================================

    LightBlockClipManagerUI.h
    Created: 17 Apr 2018 7:20:49pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "../LightBlockClipManager.h"
#include "LightBlockClipUI.h"

class LightBlockLayerTimeline;

class LightBlockClipManagerUI :
	public LayerBlockManagerUI,
	public DragAndDropTarget
{
public:
	LightBlockClipManagerUI(LightBlockLayerTimeline * timeline, LightBlockClipManager * manager);
	~LightBlockClipManagerUI();

	LightBlockClipManager * clipManager;
	LightBlockLayerTimeline * timeline;

	LayerBlockUI * createUIForItem(LayerBlock * item) override;

	bool isInterestedInDragSource(const SourceDetails &source) override;
	void itemDragEnter(const SourceDetails &source) override;
	void itemDragExit(const SourceDetails &source) override;
	void itemDragMove(const SourceDetails &source) override;
	void itemDropped(const SourceDetails &source) override;
};
