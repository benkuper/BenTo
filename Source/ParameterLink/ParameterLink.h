/*
  ==============================================================================

	ParameterLink.h
	Created: 21 Dec 2020 11:12:38pm
	Author:  bkupe

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "Prop/PropIncludes.h"

class ParameterLink :
	public Inspectable::InspectableListener
{
public:
	enum LinkType { NONE, CUSTOM_PARAM, SOURCE_BLOCK, OBJECT_ID };
	
	ParameterLink(WeakReference<Parameter> p);
	~ParameterLink();

	LinkType linkType;
	WeakReference<Parameter> parameter;

	bool isLinkable;

	//links
	WeakReference<Parameter> linkedCustomParam; //from ObjectManger custom params

	//bool replacementHasMappingInputToken;
	//String replacementString;

	void setLinkType(LinkType type);

	void setLinkedCustomParam(Parameter* p);
	var getLinkedValue(Prop* prop, int id);



	//For target parameters
	WeakReference<Controllable> getLinkedTarget(Prop* prop);
	WeakReference<ControllableContainer> getLinkedTargetContainer(Prop* prop);


	//String getReplacementString(int multiplexIndex);

	void notifyLinkUpdated();

	var getJSONData();
	void loadJSONData(var data);

	class ParameterLinkListener
	{
	public:
		virtual ~ParameterLinkListener() {}
		virtual void linkUpdated(ParameterLink* pLink) {}
	};

	ListenerList<ParameterLinkListener> parameterLinkListeners;
	void addParameterLinkListener(ParameterLinkListener* newListener) { parameterLinkListeners.add(newListener); }
	void removeParameterLinkListener(ParameterLinkListener* listener) { parameterLinkListeners.remove(listener); }

	DECLARE_ASYNC_EVENT(ParameterLink, ParameterLink, paramLink, ENUM_LIST(LINK_UPDATED, PREVIEW_UPDATED))
};

class ParamLinkContainer :
	public ControllableContainer,
	public ParameterLink::ParameterLinkListener
{
public:
	ParamLinkContainer(const String& name);
	virtual ~ParamLinkContainer();

	bool paramsCanBeLinked;

	OwnedArray<ParameterLink> paramLinks;
	HashMap<Parameter*, ParameterLink*> paramLinkMap;
	HashMap<ParameterLink*, Parameter*> linkParamMap;

	var ghostData;

	virtual void onControllableAdded(Controllable* c) override;
	virtual void onControllableRemoved(Controllable* c) override;

	virtual void parameterControlModeChanged(Parameter* p) override;

	virtual ParameterLink* getLinkedParam(Parameter* p);
	virtual var getLinkedValue(Parameter* p, Prop* prop, int id, float time = 0);

	var getParamValue(Parameter* p, float time = 0);

	virtual void linkUpdated(ParameterLink* p) override;

	template<class T>
	T* getLinkedTargetAs(TargetParameter* target, Prop* p)
	{
		if (target == nullptr) return nullptr;

		if (paramsCanBeLinked)
		{
			if (ParameterLink* pl = getLinkedParam(target)) return dynamic_cast<T*>(pl->getLinkedTarget(prop.get()));
		}

		return dynamic_cast<T*>(target->target.get());
	}

	template<class T>
	T* getLinkedTargetContainerAs(TargetParameter* target, Prop* prop)
	{
		if (paramsCanBeLinked)
		{
			if (ParameterLink* pl = getLinkedParam(target)) return dynamic_cast<T*>(pl->getLinkedTargetContainer(prop).get());
		}

		return dynamic_cast<T*>(target->targetContainer.get());
	}

	class ParamLinkContainerListener
	{
	public:
		virtual ~ParamLinkContainerListener() {}
		virtual void linkUpdated(ParamLinkContainer* container, ParameterLink* pLink) {}
		virtual void paramControlModeChanged(ParamLinkContainer* container, ParameterLink* pLink) {}
	};


	ListenerList<ParamLinkContainerListener> paramLinkContainerListeners;
	void addParamLinkContainerListener(ParamLinkContainerListener* newListener) { paramLinkContainerListeners.add(newListener); }
	void removeParamLinkContainerListener(ParamLinkContainerListener* listener) { paramLinkContainerListeners.remove(listener); }

	virtual var getJSONData() override;
	virtual void loadJSONDataInternal(var data) override;

	virtual InspectableEditor* getEditorInternal(bool isRoot, Array<Inspectable*> inspectables = {}) override;
};