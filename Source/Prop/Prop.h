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

#define PROP_PING_TIMERID 0
#define PROP_PINGPONG_TIMERID 1

class Prop :
	public BaseItem,
	public Inspectable::InspectableListener,
	public Thread,
	public MultiTimer,
	public LightBlockColorProvider::ProviderListener
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
	BoolParameter* isConnected;
	
	BoolParameter* twoWayConnected;
	bool receivedPongSinceLastPingSent;

	BoolParameter * findPropMode;
	Trigger* powerOffTrigger;
	Trigger* resetTrigger;

	ControllableContainer sensorsCC;
	FloatParameter * battery;

	ControllableContainer bakingCC;
	FloatParameter * bakeStartTime;
	FloatParameter * bakeEndTime;
	IntParameter * bakeFrequency;
	Trigger * bakeAndUploadTrigger;
	Trigger * bakeAndExportTrigger;
	StringParameter * bakeFileName;
	BoolParameter * bakeMode;

	BoolParameter * sendCompressedFile;
	BoolParameter * isBaking;
	
	BoolParameter * isUploading;
	FloatParameter * bakingProgress;
	FloatParameter * uploadProgress;

	enum AfterBakeAction { UPLOAD, EXPORT, NOTHING };
	AfterBakeAction afterBake;

	BaseColorProvider* providerToBake;

	float seekBakeTime;

	File exportFile;
	
	Array<Colour> colors;

	std::unique_ptr<LightBlock> currentBlock;
	TargetParameter * activeProvider; 

	int previousID; //for swapping
	int updateRate;

	//ping

	virtual void clearItem() override;

	void registerFamily(StringRef familyName);

	void setBlockFromProvider(LightBlockColorProvider * model);

	void update();

	void onContainerParameterChangedInternal(Parameter * p) override;
	void onControllableFeedbackUpdateInternal(ControllableContainer * cc, Controllable *c) override;
	void inspectableDestroyed(Inspectable *) override;

	void sendColorsToProp(bool forceSend = false);
	virtual void sendColorsToPropInternal() {}

	static void fillTypeOptions(EnumParameter * p);
	
	virtual void initBaking(BaseColorProvider* block, AfterBakeAction afterBakeAction);
	virtual BakeData bakeBlock();

	virtual void uploadBakedData(BakeData data);
	virtual void exportBakedData(BakeData data);

	virtual void loadBake(StringRef /*fileName*/, bool /*autoPlay*/) {}
	virtual void playBake(float /*time */ = -1) {}
	virtual void pauseBakePlaying() {}
	virtual void resumeBakePlaying() {}
	virtual void seekBakePlaying(float /*time */) {}
	virtual void stopBakePlaying() {}
	virtual void sendShowPropID(bool value) {}

	void providerBakeControlUpdate(LightBlockColorProvider::BakeControl control, var data) override;

	virtual void powerOffProp() {}
	virtual void resetProp() {}

	virtual void handlePing(bool isPong = false);
	virtual void sendPing() {}
	virtual void timerCallback(int timerID) override;
	
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