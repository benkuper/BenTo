/*
  ==============================================================================

    LightBlockLayer.h
    Created: 17 Apr 2018 5:10:36pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "LightBlockClipManager.h"

class LightBlockLayer :
	public SequenceLayer
{
public:
	LightBlockLayer(Sequence * s, var params = var());
	~LightBlockLayer();

	LightBlockClipManager blockClipManager;

	BoolParameter * defaultLayer;
	BoolParameter * globalLayer;
	IntParameter * targetId;

	Array<Colour> getColors(int id, int resolution, double time, var params);

	SequenceLayerPanel * getPanel() override;
	SequenceLayerTimeline * getTimelineUI() override;

	String getTypeString() const override { return "Blocks"; }


	var getJSONData() override;
	void loadJSONDataInternal(var data) override;

	static LightBlockLayer * create(Sequence * s, var params) { return new LightBlockLayer(s, params);  }
};