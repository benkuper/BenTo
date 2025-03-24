/*
  ==============================================================================

	LightBlockLayer.h
	Created: 17 Apr 2018 5:10:36pm
	Author:  Ben

  ==============================================================================
*/

#pragma once

class LightBlockLayer :
	public SequenceLayer,
	public PropTargetFilterManager::FilterManagerListener,
	public EngineListener
{
public:
	LightBlockLayer(Sequence* s, var params = var());
	~LightBlockLayer();

	LightBlockClipManager blockClipManager;

	enum BlendMode { ADD, ALPHA, MASK };

	BoolParameter* defaultLayer;
	EnumParameter* blendMode;
	Point2DParameter* positionRemap;

	std::unique_ptr<PropTargetFilterManager> filterManager;
	IntParameter* previewID;

	Array<Colour> getColors(Prop* p, double time, var params);

	template <class T>
	Array<LightBlockClip*> getClipsOfTypeForProp(Prop* p)
	{
		Array<LightBlockClip*> result;

		for (auto& b : blockClipManager.items)
		{
			LightBlockClip* lb = (LightBlockClip*)b;

			if (lb->currentBlock == nullptr) continue;
			if (lb->currentBlock->provider == nullptr) continue;
			if (dynamic_cast<T*>(lb->currentBlock->provider.get()) == nullptr) continue;

			int localID = lb->filterManager->getTargetIDForProp(p);
			if (localID == -1) continue;

			result.add(lb);
		}
		return result;
	}

	void updateLinkedProps();
	void filtersChanged() override;

	void sequenceCurrentTimeChanged(Sequence* s, float prevTime, bool) override;
	void onContainerParameterChangedInternal(Parameter* p) override;

	bool paste() override;
	void selectAll(bool addToSelection = false) override;

	virtual void getSnapTimes(Array<float>* arrayToFill) override;

	var getJSONData(bool includeNonOverriden = false) override;
	void loadJSONDataInternal(var data) override;
	void endLoadFile() override;


	SequenceLayerPanel* getPanel() override;
	SequenceLayerTimeline* getTimelineUI() override;

	String getTypeString() const override { return "Blocks"; }

	static LightBlockLayer* create(Sequence* s, var params) { return new LightBlockLayer(s, params); }
};

