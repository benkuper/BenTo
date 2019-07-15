/*
  ==============================================================================

	LightBlock.h
	Created: 10 Apr 2018 6:56:47pm
	Author:  Ben

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "model/LightBlockModel.h"


class LightBlock :
	public BaseItem,
	public BaseColorProvider,
	public LightBlockColorProvider::ProviderListener
{
public:
	LightBlock(LightBlockColorProvider * provider);
	virtual ~LightBlock();

	WeakReference<LightBlockColorProvider> provider;
	ControllableContainer paramsContainer;

	var paramsLoadData;

	Array<Colour> getColors(Prop* p, double time, var params) override;
	BakeData getBakeDataForProp(Prop* prop) override;

	void rebuildArgsFromModel();

	void providerParametersChanged(LightBlockColorProvider *) override;
	void providerParameterValueUpdated(LightBlockColorProvider *, Parameter * p) override;
	virtual void parameterControlModeChanged(Parameter * p) override;


	var getJSONData() override;
	void loadJSONDataInternal(var data) override;
	
	class  LightBlockListener
	{
	public:
		virtual ~LightBlockListener() {}
		virtual void blockParamControlModeChanged(Parameter * p) {}
		virtual void blockBakingProgress(float progress) {}
		//virtual void colorsUpdated() {}
	};

	ListenerList<LightBlockListener> blockListeners;
	void addLightBlockListener(LightBlockListener* newListener) { blockListeners.add(newListener); }
	void removeLightBlockListener(LightBlockListener* listener) { blockListeners.remove(listener); }
	


	static LightBlock * create(LightBlockColorProvider * provider) { return new LightBlock(provider); }

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LightBlock)
};