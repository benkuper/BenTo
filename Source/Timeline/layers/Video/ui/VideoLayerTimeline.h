/*
  ==============================================================================

    VideoLayerTimeline.h
    Created: 4 Jan 2021 11:07:35pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "../VideoLayer.h"

class VideoLayerTimeline;

class VideoLayerClipManagerUI :
	public LayerBlockManagerUI
{
public:
	VideoLayerClipManagerUI(VideoLayerTimeline* timeline, AudioLayerClipManager* manager);
	~VideoLayerClipManagerUI();

	void mouseDoubleClick(const MouseEvent& e) override;
	void addClipWithFileChooserAt(float position);
};

class VideoLayerTimeline :
	public SequenceLayerTimeline
{
public:
	VideoLayerTimeline(VideoLayer* layer);
	~VideoLayerTimeline();

	VideoLayer* videoLayer;
	std::unique_ptr<VideoLayerClipManagerUI> cmMUI;

	void resized() override;
	void updateContent() override;
	virtual void updateMiniModeUI() override;

	virtual void addSelectableComponentsAndInspectables(Array<Component*>& selectables, Array<Inspectable*>& inspectables) override;
};