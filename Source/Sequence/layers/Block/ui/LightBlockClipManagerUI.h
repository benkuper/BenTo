/*
  ==============================================================================

    LightBlockClipManagerUI.h
    Created: 17 Apr 2018 7:20:49pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

class LightBlockLayerTimeline;

class LightBlockClipManagerUI :
	public LayerBlockManagerUI
{
public:
	LightBlockClipManagerUI(LightBlockLayerTimeline * timeline, LightBlockClipManager * manager);
	~LightBlockClipManagerUI();

	LightBlockClipManager * clipManager;
	LightBlockLayerTimeline * timeline;

	int dropClipX;

	void paint(Graphics &g) override;

	LayerBlockUI * createUIForItem(LayerBlock * item) override;

	void itemDragEnter(const SourceDetails &source) override;
	void itemDragExit(const SourceDetails &source) override;
	void itemDragMove(const SourceDetails &source) override;
	void itemDropped(const SourceDetails &source) override;
};
