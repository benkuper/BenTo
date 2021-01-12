/*
  ==============================================================================

    LightBlockLayer.h
    Created: 17 Apr 2018 5:10:36pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "LightBlockClipManager.h"
#include "Prop/TargetFilter/PropTargetFilterManager.h"

class LightBlockLayer :
	public SequenceLayer,
	public PropTargetFilterManager::FilterManagerListener,
	public EngineListener
{
public:
	LightBlockLayer(Sequence * s, var params = var());
	~LightBlockLayer();

	LightBlockClipManager blockClipManager;

	enum BlendMode { ADD, ALPHA, MASK };
	
	BoolParameter * defaultLayer;
	EnumParameter* blendMode;
	
	std::unique_ptr<PropTargetFilterManager> filterManager;
	IntParameter * previewID;

	Array<Colour> getColors(Prop * p, double time, var params);

	void updateLinkedProps();
	void filtersChanged() override;

	void sequenceCurrentTimeChanged(Sequence* s, float prevTime, bool) override;
	void onContainerParameterChangedInternal(Parameter* p) override;
	
	bool paste() override;
	void selectAll(bool addToSelection = false) override;

	var getJSONData() override;
	void loadJSONDataInternal(var data) override;
	void endLoadFile() override;


	SequenceLayerPanel * getPanel() override;
	SequenceLayerTimeline * getTimelineUI() override;

	String getTypeString() const override { return "Blocks"; }

	static LightBlockLayer * create(Sequence * s, var params) { return new LightBlockLayer(s, params);  }
};