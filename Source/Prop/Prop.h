/*
  ==============================================================================

	Prop.h
	Created: 10 Apr 2018 6:59:25pm
	Author:  Ben

  ==============================================================================
*/

#pragma once

class PropFamily;
class RGBPropComponent;

#define PROP_PING_TIMERID 0

class Prop :
	public BaseItem,
	public Inspectable::InspectableListener,
	public Thread,
	public MultiTimer,
	public LightBlockColorProvider::ProviderListener
{
public:
	enum Shape { CLUB, BALL, POI, HOOP, RING, BUGGENG, BOX, CUSTOM };

	Prop(var params = var());
	virtual ~Prop();


	String deviceID;
	PropFamily* family;
	String customType;

	BoolParameter* logIncoming;
	BoolParameter* logOutgoing;

	ControllableContainer generalCC;
	IntParameter* globalID;
	IntParameter* resolution;
	EnumParameter* type;

	ControllableContainer connectionCC;
	BoolParameter* isConnected;

	bool pingEnabled;
	bool receivedPongSinceLastPingSent;

	BoolParameter* findPropMode;

	ControllableContainer controlsCC;
	Trigger* powerOffTrigger;
	Trigger* restartTrigger;

	HashMap<String, PropComponent*> components;
	RGBPropComponent* rgbComponent;

	ControllableContainer bakingCC;
	FloatParameter* bakeStartTime;
	FloatParameter* bakeEndTime;
	IntParameter* bakeFrequency;
	Trigger* bakeAndUploadTrigger;
	Trigger* bakeAndExportTrigger;
	StringParameter* bakeFileName;
	BoolParameter* bakeMode;

	BoolParameter* sendCompressedFile;
	BoolParameter* isBaking;

	BoolParameter* isUploading;
	FloatParameter* bakingProgress;
	FloatParameter* uploadProgress;

	enum AfterBakeAction { UPLOAD, EXPORT, NOTHING };
	AfterBakeAction afterBake;

	BaseColorProvider* providerToBake;

	float seekBakeTime;

	File exportFile;

	Array<Colour> colors;

	std::unique_ptr<LightBlock> currentBlock;
	TargetParameter* activeProvider;

	int previousID; //for swapping
	int updateRate;

	Array<File, CriticalSection> filesToUpload;

	//ping
	virtual void clearItem() override;

	void registerFamily(StringRef familyName);

	void setBlockFromProvider(LightBlockColorProvider* model);

	void update();

	void onContainerParameterChangedInternal(Parameter* p) override;
	void onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c) override;
	void inspectableDestroyed(Inspectable*) override;

	void sendColorsToProp(bool forceSend = false);
	virtual void sendColorsToPropInternal() {}

	static void fillTypeOptions(EnumParameter* p);

	virtual void initBaking(BaseColorProvider* block, AfterBakeAction afterBakeAction);
	virtual BakeData bakeBlock();

	virtual void uploadBakedData(BakeData data);
	virtual void exportBakedData(BakeData data);
	virtual void addFileToUpload(File f);
	virtual void uploadFileQueue();
	virtual void uploadFile(File f);


	virtual void loadBake(StringRef /*fileName*/, bool /*autoPlay*/) {}
	virtual void playBake(float /*time */ = 0, bool /* loop */ = false) {}
	virtual void pauseBakePlaying() {}
	virtual void resumeBakePlaying() {}
	virtual void seekBakePlaying(float /*time */) {}
	virtual void stopBakePlaying() {}
	virtual void sendShowPropID(bool value) {}

	void providerBakeControlUpdate(LightBlockColorProvider::BakeControl control, var data) override;

	void sendControlToProp(String message, var value = var());
	virtual void sendControlToPropInternal(String message, var value = var()) {} //to be overriden

	virtual void powerOffProp() {}
	virtual void restartProp() {}

	virtual void handleOSCMessage(const OSCMessage& m);

	virtual void handlePong();
	void sendPing();
	virtual void sendPingInternal() {}
	virtual void timerCallback(int timerID) override;

	void setupComponentsJSONDefinition(var def);
	void addComponent(PropComponent* pc);

	PropComponent* getComponent(const String& name);

	var getJSONData() override;
	void loadJSONDataInternal(var data) override;


	InspectableEditor* getEditorInternal(bool isRoot, Array<Inspectable*> inspectables = {}) override;

	//Listener
	class  PropListener
	{
	public:
		/** Destructor. */
		virtual ~PropListener() {}
		virtual void propBlockChanged(Prop* /*prop*/) {}
		virtual void propIDChanged(Prop*, int) {}
		virtual void colorsUpdated(Prop*) {}
	};

	ListenerList<PropListener> propListeners;
	void addPropListener(PropListener* newListener) { propListeners.add(newListener); }
	void removePropListener(PropListener* listener) { propListeners.remove(listener); }

	// ASYNC
	class  PropEvent
	{
	public:
		enum Type { BLOCK_CHANGED, COLORS_UPDATED };

		PropEvent(Type t, Prop* p, var v = var()) :
			type(t), prop(p), value(v) {}

		Type type;
		Prop* prop;
		var value;
	};

	QueuedNotifier<PropEvent> propNotifier;
	typedef QueuedNotifier<PropEvent>::Listener AsyncListener;

	void addAsyncPropListener(AsyncListener* newListener) { propNotifier.addListener(newListener); }
	void addAsyncCoalescedPropListener(AsyncListener* newListener) { propNotifier.addAsyncCoalescedListener(newListener); }
	void removeAsyncPropListener(AsyncListener* listener) { propNotifier.removeListener(listener); }

	virtual void run() override;

	virtual String getTypeString() const { return customType; }
	static Prop* create(var params) { return new Prop(params); }

private:
	WeakReference<Prop>::Master masterReference;
	friend class WeakReference<Prop>;
};