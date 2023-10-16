/*
  ==============================================================================

	Prop.h
	Created: 10 Apr 2018 6:59:25pm
	Author:  Ben

  ==============================================================================
*/

#pragma once

class Prop :
	public BaseItem,
	public Inspectable::InspectableListener,
	public Thread,
	public LightBlockColorProvider::ProviderListener
{
public:
	enum Shape { CLUB, BALL, POI, HOOP, RING, BUGGENG, BOX, CUSTOM };

	Prop(var params = var());
	virtual ~Prop();


	String deviceID;
	IntParameter* globalID;


	BoolParameter* logIncoming;
	BoolParameter* logOutgoing;

	ControllableContainer generalCC;
	IntParameter* resolution;
	FloatParameter* brightness;

	IntParameter* resolutionRef;
	EnumParameter* shape;
	bool invertLedsInUI;

	ControllableContainer connectionCC;
	BoolParameter* isConnected;

	BoolParameter* findPropMode;

	ControllableContainer controlsCC;
	Trigger* powerOffTrigger;
	Trigger* restartTrigger;

	ControllableContainer playbackCC;
	FloatParameter* playbackStartTime;
	FloatParameter* playbackEndTime;
	IntParameter* playbackFrequency;
	Trigger* uploadPlaybackTrigger;
	Trigger* exportPlaybackTrigger;
	StringParameter* playbackFileName;
	BoolParameter* playbackMode;

	bool useAlphaInPlaybackData;
	BoolParameter* sendCompressedFile;
	BoolParameter* isGeneratingPlayback;

	BoolParameter* isUploading;
	FloatParameter* playbackGenProgress;
	FloatParameter* uploadProgress;

	enum AfterPlaybackGenAction { UPLOAD, EXPORT, NOTHING };
	AfterPlaybackGenAction afterGeneratePlayback;

	BaseColorProvider* playbackProvider;

	float seekPlaybackTime;

	File exportFile;

	SpinLock colorLock;
	Array<Colour> colors;

	std::unique_ptr<LightBlock> currentBlock;
	TargetParameter* activeProvider;

	int previousID; //for swapping
	int updateRate;

	Array<File, CriticalSection> filesToUpload;


	virtual void clearItem() override;

	void setBlockFromProvider(LightBlockColorProvider* model);
	int getResolution();
	void update();

	void onContainerParameterChangedInternal(Parameter* p) override;
	void onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c) override;
	void inspectableDestroyed(Inspectable*) override;

	void sendColorsToProp(bool forceSend = false);
	virtual void sendColorsToPropInternal() {}

	static void fillTypeOptions(EnumParameter* p);

	virtual void initGeneratePlayback(BaseColorProvider* block, AfterPlaybackGenAction afterPlaybackGenAction);
	virtual PlaybackData generatePlayback();

	virtual void uploadPlaybackData(PlaybackData data);
	virtual void exportPlaybackData(PlaybackData data);
	virtual void addFileToUpload(File f);
	virtual void uploadFileQueue();
	virtual void uploadFile(File f);

	virtual void loadPlayback(StringRef /*fileName*/, bool /*autoPlay*/) {}
	virtual void playPlayback(float /*time */ = 0, bool /* loop */ = false) {}
	virtual void pausePlaybackPlaying() {}
	virtual void resumePlaybackPlaying() {}
	virtual void seekPlaybackPlaying(float /*time */) {}
	virtual void stopPlaybackPlaying() {}
	virtual void sendShowPropID(bool value) {}

	void providerPlaybackControlUpdate(LightBlockColorProvider::PlaybackControl control, var data) override;

	virtual void powerOffProp() {}
	virtual void restartProp() {}

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

	DECLARE_TYPE("Prop");

private:
	WeakReference<Prop>::Master masterReference;
	friend class WeakReference<Prop>;
};