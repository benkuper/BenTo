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
	public LightBlockColorProvider::ProviderListener
{
public:
	LightBlock(LightBlockColorProvider * provider);
	virtual ~LightBlock();

	WeakReference<LightBlockColorProvider> provider;
	ControllableContainer paramsContainer;

	ParameterAutomationManager automationsManager;

	Array<Colour> getColors(int id, int resolution, float time, var params);

	void rebuildArgsFromModel();

	void providerParametersChanged(LightBlockColorProvider *) override;
	void providerParameterValueUpdated(LightBlockColorProvider *, Parameter * p) override;
	 
	var getJSONData() override;
	void loadJSONDataInternal(var data) override;



	/*
	class  LightBlockListener
	{
	public:
		virtual ~LightBlockListener() {}
		virtual void colorsUpdated() {}
	};

	ListenerList<LightBlockListener> blockListeners;
	void addLightBlockListener(LightBlockListener* newListener) { blockListeners.add(newListener); }
	void removeLightBlockListener(LightBlockListener* listener) { blockListeners.remove(listener); }
	*/


	static LightBlock * create(LightBlockColorProvider * provider) { return new LightBlock(provider); }

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LightBlock)
};