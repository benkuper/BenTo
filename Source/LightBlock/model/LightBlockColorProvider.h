/*
  ==============================================================================

    ModelListener.h
    Created: 12 Apr 2018 1:47:03pm
    Author:  Ben

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"

class LightBlock;
class Prop;

class LightBlockColorProvider : 
	public BaseItem
{
public:
	LightBlockColorProvider(const String &name = "Provider", bool canBeDisabled = true, bool canHaveScripts = false);
	virtual ~LightBlockColorProvider();
	
	Trigger * assignToAll;

	virtual Array<WeakReference<Controllable>> getModelParameters() = 0;
	virtual Array<Colour> getColors(Prop * p, double time, var params) = 0;

	//Bake info
	enum BakeControl { PLAY, PAUSE, STOP, SEEK };

	void onContainerTriggerTriggered(Trigger *) override;

	class ProviderListener
	{
	public:
		virtual ~ProviderListener() {}
		virtual void providerParametersChanged(LightBlockColorProvider *) {}
		virtual void providerParameterValueUpdated(LightBlockColorProvider *, Parameter *) {}
		virtual void providerBakeControlUpdate(BakeControl control, var data = var()) {}
	};

	ListenerList<ProviderListener> providerListeners;
	void addColorProviderListener(ProviderListener* newListener) { providerListeners.add(newListener); }
	void removeColorProviderListener(ProviderListener* listener) { providerListeners.remove(listener); }

private:
	WeakReference<LightBlockColorProvider>::Master masterReference;
	friend class WeakReference<LightBlockColorProvider>;
};
