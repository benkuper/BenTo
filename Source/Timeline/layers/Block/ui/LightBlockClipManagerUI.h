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
	public BaseManagerUI<LightBlockClipManager, LightBlockClip, LightBlockClipUI>,
	public LightBlockClipUI::ClipUIListener
{
public:
	LightBlockClipManagerUI(LightBlockLayerTimeline * timeline, LightBlockClipManager * manager);
	~LightBlockClipManagerUI();

	LightBlockLayerTimeline * timeline;

	void resized() override;
	void updateContent();

	void placeClipUI(LightBlockClipUI * cui);

	void addItemUIInternal(LightBlockClipUI * cui) override;
	void removeItemUIInternal(LightBlockClipUI * cui) override;

	void mouseDoubleClick(const MouseEvent &e) override;

	void clipUITimeChanged(LightBlockClipUI * cui) override;
	void clipUIDragged(LightBlockClipUI * cui, const MouseEvent &) override;
};
