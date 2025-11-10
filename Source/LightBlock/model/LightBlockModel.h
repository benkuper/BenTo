/*
  ==============================================================================

    LightBlockModel.h
    Created: 10 Apr 2018 7:12:39pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

class LightBlock;
class LightBlockModelUI;
class Prop;

class LightBlockModel :
	public LightBlockColorProvider
{
public:
	
	LightBlockModel(const String &name = "LightBlockModel", var params = var(), ProviderType providerType = SOURCE);
	~LightBlockModel();

	//ui
	String customThumbnailPath;
	Trigger* assignToAll;

	bool isBeingEdited;
	void setBeingEdited(bool value);

	void clear() override;


	std::unique_ptr<ControllableContainer> paramsContainer;
	LightBlockModelPresetManager presetManager;

	virtual Array<WeakReference<Controllable>> getModelParameters() override;

	virtual Array<Colour> getColors(Prop * p, double time, var params) override;
	virtual void getColorsInternal(Array<Colour> * result, Prop * p, double time, int id, int resolution, var params) {}


	template<class T>
	T getParamValue(Parameter * p, var params);

	void onContainerTriggerTriggered(Trigger* t) override;
	void onControllableFeedbackUpdateInternal(ControllableContainer * cc, Controllable * c) override;
	void childStructureChanged(ControllableContainer * cc) override;

	//ui
	void setCustomThumbnail(String path);

	var getJSONData(bool includeNonOverriden = false) override;
	void loadJSONDataInternal(var data) override;


	Array<Prop*> getAssignedProps();

	
	virtual LightBlockModelUI * createUI();

	//Listener
	class  ModelListener
	{
	public:
		/** Destructor. */
		virtual ~ModelListener() {}
		virtual void customThumbnailChanged(LightBlockModel * /*model*/) {}
	};

	ListenerList<ModelListener> modelListeners;
	void addModelListener(ModelListener* newListener) { modelListeners.add(newListener); }
	void removeModelListener(ModelListener* listener) { modelListeners.remove(listener); }


	// ASYNC
	class  ModelEvent
	{
	public:
		enum Type { CUSTOM_THUMBNAIL_CHANGED, EDITING_STATE_CHANGED};

		ModelEvent(Type t, LightBlockModel * m) :
			type(t), model(m) {}

		Type type;
		LightBlockModel * model;
	};

	QueuedNotifier<ModelEvent> modelNotifier;
	typedef QueuedNotifier<ModelEvent>::Listener AsyncListener;

	void addAsyncModelListener(AsyncListener* newListener) { modelNotifier.addListener(newListener); }
	void addAsyncCoalescedModelListener(AsyncListener* newListener) { modelNotifier.addAsyncCoalescedListener(newListener); }
	void removeAsyncModelListener(AsyncListener* listener) { modelNotifier.removeListener(listener); }

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LightBlockModel)
};

template<class T>
T LightBlockModel::getParamValue(Parameter * p, var params)
{
	return (T)params.getProperty(p->shortName, p->getValue());
}
