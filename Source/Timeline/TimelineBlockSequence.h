/*
  ==============================================================================

    TimelineBlockSequence.h
    Created: 17 Apr 2018 8:30:13pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "Prop/Cluster/PropClusterGroupManager.h"

class LightBlock;
class LightBlockLayer;
class Prop;

class TimelineBlockSequence :
	public Sequence,
	public SequenceLayerManager::ManagerListener,
	public PropClusterGroupManager::ManagerListener
{
public:
	TimelineBlockSequence();
	~TimelineBlockSequence();

	Factory<SequenceLayer> layerFactory;

	BoolParameter* identityMode;
	EnumParameter* identityClusterGroup;
	PropClusterGroup* currentIdentityGroup;

	PropClusterGroupManager clusterGroupManager;
	
	Array<Colour> getColors(Prop * p, double time, var params);
	Array<LightBlockLayer *> getLayersForProp(Prop * p, bool includeDisabled = false);

	void itemAdded(SequenceLayer * s) override;

	void itemAdded(PropClusterGroup* g) override;
	void itemRemoved(PropClusterGroup* g) override;

	void updateGroupList();

	var getJSONData() override;
	void loadJSONDataInternal(var data) override;

	void onContainerParameterChangedInternal(Parameter* p) override; 
	void onControllableFeedbackUpdateInternal(ControllableContainer * cc, Controllable * c) override;
};