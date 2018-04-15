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
	public LightBlockColorProvider::ProviderListener,
	public Timer
{
public:
	LightBlock(LightBlockColorProvider * provider, Prop * prop, var params = var());
	virtual ~LightBlock();

	Prop * prop; 
	WeakReference<LightBlockColorProvider> provider;
	
	ControllableContainer paramsContainer;

	void update();

	void timerCallback();

	void rebuildArgsFromModel();
	void providerParametersChanged(LightBlockColorProvider *) override;
	 
	var getJSONData() override;
	void loadJSONDataInternal(var data) override;

	class  LightBlockListener
	{
	public:
		/** Destructor. */
		virtual ~LightBlockListener() {}
		virtual void colorsUpdated() {}
	};

	ListenerList<LightBlockListener> blockListeners;
	void addLightBlockListener(LightBlockListener* newListener) { blockListeners.add(newListener); }
	void removeLightBlockListener(LightBlockListener* listener) { blockListeners.remove(listener); }



	static LightBlock * create(LightBlockModel * model, Prop * prop, var params) { return new LightBlock(model, prop, params); }


	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LightBlock)
};