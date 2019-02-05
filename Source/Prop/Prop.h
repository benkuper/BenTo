/*
  ==============================================================================

    Prop.h
    Created: 10 Apr 2018 6:59:25pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "LightBlock/LightBlock.h"

class PropFamily;

class Prop :
	public BaseItem,
	public Inspectable::InspectableListener,
	public Thread
{
public:
	enum Shape { CLUB, BALL, POI, HOOP, RING, BUGGENG, BOX };

	Prop(StringRef name = "", StringRef familyName = "", var params = var());
	virtual ~Prop();

	String deviceID;
	PropFamily * family;

	ControllableContainer generalCC;
	IntParameter * globalID;
	IntParameter * resolution;
	EnumParameter * type;
	
	ControllableContainer ioCC;
	BoolParameter * findPropMode;

	ControllableContainer sensorsCC;
	FloatParameter * battery;

	//GenericControllableManager customParamsCC;

	ControllableContainer bakingCC;
	FloatParameter * bakeStartTime;
	FloatParameter * bakeEndTime;
	IntParameter * bakeFrequency;
	Trigger * bakeAndUploadTrigger;
	BoolParameter * isBaking;

	Array<Colour> colors;

	ScopedPointer<LightBlock> currentBlock;
	TargetParameter * activeProvider; 

	int previousID; //for swapping
	int updateRate;

	virtual void clearItem() override;

	void registerFamily(StringRef familyName);

	void setBlockFromProvider(LightBlockColorProvider * model);

	void update();

	void onContainerParameterChangedInternal(Parameter * p) override;
	void onControllableFeedbackUpdateInternal(ControllableContainer * cc, Controllable *c) override;
	void onContainerTriggerTriggered(Trigger * t) override;
	void inspectableDestroyed(Inspectable *) override;

	void sendColorsToProp(bool forceSend = false);
	virtual void sendColorsToPropInternal() {}

	static void fillTypeOptions(EnumParameter * p);

	struct TimedColors
	{
		double time;
		Array<Colour> colors;
	};
	
	virtual Array<TimedColors> bakeCurrentBlock();
	virtual void uploadCurrentBlock(Array<TimedColors> bakedColors);

	var getJSONData() override;
	void loadJSONDataInternal(var data) override;


	//Listener
	class  PropListener
	{
	public:
		/** Destructor. */
		virtual ~PropListener() {}
		virtual void propBlockChanged(Prop * /*prop*/) {}
		virtual void propIDChanged(Prop *, int) {}
		virtual void colorsUpdated(Prop *) {}
	};

	ListenerList<PropListener> propListeners;
	void addPropListener(PropListener* newListener) { propListeners.add(newListener); }
	void removePropListener(PropListener* listener) { propListeners.remove(listener); }
	
	// ASYNC
	class  PropEvent
	{
	public:
		enum Type { BLOCK_CHANGED, COLORS_UPDATED };

		PropEvent(Type t, Prop * p, var v = var()) :
			type(t), prop(p), value(v) {}

		Type type;
		Prop * prop;
		var value;
	};

	QueuedNotifier<PropEvent> propNotifier;
	typedef QueuedNotifier<PropEvent>::Listener AsyncListener;

	void addAsyncPropListener(AsyncListener* newListener) { propNotifier.addListener(newListener); }
	void addAsyncCoalescedPropListener(AsyncListener* newListener) { propNotifier.addAsyncCoalescedListener(newListener); }
	void removeAsyncPropListener(AsyncListener* listener) { propNotifier.removeListener(listener); }

	// Inherited via Thread
	virtual void run() override;
};