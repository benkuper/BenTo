/*
  ==============================================================================

    LightBlockClip.h
    Created: 17 Apr 2018 5:10:24pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "LightBlock/LightBlock.h"
#include "LightBlock/model/blocks/filters/LightBlockFilter.h"

class LightBlockLayer;
class PropTargetFilterManager;

class LightBlockClip :
	public LayerBlock,
	public LightBlock::LightBlockListener
{
public:
	LightBlockClip(LightBlockLayer * layer = nullptr);
	~LightBlockClip();

	LightBlockLayer * layer;
	std::unique_ptr<LightBlock> currentBlock;
	
	TargetParameter * activeProvider;

	FloatParameter * fadeIn;
	FloatParameter * fadeOut;

	std::unique_ptr<PropTargetFilterManager> filterManager;
	BaseManager<LightBlock> effects;

	void setBlockFromProvider(LightBlockColorProvider * provider);
	Array<Colour> getColors(Prop * p, double absoluteTime, var params);

	void addFilterFromProvider(LightBlockFilter * provider);
	
	virtual void blockParamControlModeChanged(Parameter *p) override;

	void setCoreLength(float value, bool stretch = false, bool stickToCoreEnd = false) override;

	void notifyUpdatePreview();

	void handleEnterExit(bool enter, Array<Prop*> filteredProps);

	void onContainerParameterChangedInternal(Parameter * p) override;
	virtual void controllableStateChanged(Controllable* c) override;

	var getJSONData() override;
	void loadJSONDataInternal(var data) override;

	String getTypeString() const override { return "LightBlockClip"; }


	//Listener
	//Listener
	class  ClipListener
	{
	public:
		/** Destructor. */
		virtual ~ClipListener() {}
		virtual void clipFadesChanged (LightBlockClip *) {}
	};

	ListenerList<ClipListener> clipListeners;
	void addClipListener(ClipListener* newListener) { clipListeners.add(newListener); }
	void removeClipListener(ClipListener* listener) { clipListeners.remove(listener); }

	// ASYNC
	class  ClipEvent
	{
	public:
		enum Type { FADES_CHANGED, REGENERATE_PREVIEW };

		ClipEvent(Type t, LightBlockClip* p, var v = var()) :
			type(t), clip(p), value(v) {}

		Type type;
		LightBlockClip* clip;
		var value;
	};

	QueuedNotifier<ClipEvent> clipNotifier;
	typedef QueuedNotifier<ClipEvent>::Listener AsyncListener;

	void addAsyncClipListener(AsyncListener* newListener) { clipNotifier.addListener(newListener); }
	void addAsyncCoalescedClipListener(AsyncListener* newListener) { clipNotifier.addAsyncCoalescedListener(newListener); }
	void removeAsyncClipListener(AsyncListener* listener) { clipNotifier.removeListener(listener); }
};