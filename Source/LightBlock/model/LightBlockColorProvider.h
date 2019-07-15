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

//Baking
struct BakeData
{
	BakeData(StringRef name = "default", float startTime = 0, float endTime = 10, int fps = 100, var metaData = new DynamicObject()) :
		name(String(name)), startTime(startTime), endTime(endTime), numFrames(0), fps(fps), metaData(metaData)
	{

	}
	String name;
	float startTime;
	float endTime;
	int numFrames;
	int fps;
	MemoryBlock data;
	var metaData;
};


class BaseColorProvider
{
public:
	virtual Array<Colour> getColors(Prop* p, double time, var params) = 0;
	virtual BakeData getBakeDataForProp(Prop*) = 0;
};

class LightBlockColorProvider : 
	public BaseItem,
	public BaseColorProvider
{
public:
	LightBlockColorProvider(const String &name = "Provider", bool canBeDisabled = true, bool canHaveScripts = false);
	virtual ~LightBlockColorProvider();
	
	Trigger * assignToAll;


	//Bake info
	enum BakeControl { PLAY, PAUSE, STOP, SEEK };
	
	virtual Array<WeakReference<Controllable>> getModelParameters() = 0;

	virtual Array<Colour> getColors(Prop* p, double time, var params) override;
	virtual BakeData getBakeDataForProp(Prop*) override;

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
