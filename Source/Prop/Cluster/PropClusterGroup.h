/*
  ==============================================================================

    PropClusterGroup.h
    Created: 5 Feb 2019 11:12:42am
    Author:  jonglissimo

  ==============================================================================
*/

#pragma once

class PropClusterGroup :
	public BaseItem
{
public:
	PropClusterGroup();
	~PropClusterGroup();

	BoolParameter * sendFeedback;

	BaseManager<PropCluster> clusterManager;

	PropCluster* getClusterForProp(Prop* p, int& localID);
	int getClusterIDForProp(Prop * p);

	Array<Colour> getColorsForProp(Prop* p);
	
	int getLocalPropID(Prop * p);

	var getJSONData() override;
	void loadJSONDataInternal(var data) override;
};