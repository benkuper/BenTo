/*
  ==============================================================================

    PropClusterGroup.cpp
    Created: 5 Feb 2019 11:12:42am
    Author:  jonglissimo

  ==============================================================================
*/

#include "PropClusterGroup.h"

PropClusterGroup::PropClusterGroup() :
	BaseItem("Cluster Group"),
	clusterManager("Clusters")
{
	sendFeedback = addBoolParameter("Send Feedback", "If checked, Prop changes will be sent using their local ID", false);

	//clusterManager.hideEditorHeader = true;
	clusterManager.skipControllableNameInAddress = true;
	clusterManager.selectItemWhenCreated = false;
	clusterManager.editorCanBeCollapsed = false;

	clusterManager.addItem(); //add one item here

	addChildControllableContainer(&clusterManager);
}

PropClusterGroup::~PropClusterGroup()
  {
  }

PropCluster * PropClusterGroup::getClusterForProp(Prop * p, int &localID)
{
	for (auto &c : clusterManager.items)
	{
		int id = c->getLocalPropID(p);
		if (id >= 0)
		{
			localID = id;
			return c;
		}
	}

	return nullptr;
}

int PropClusterGroup::getLocalPropID(Prop * p)
{
	for (auto &c : clusterManager.items)
	{
		int id = c->getLocalPropID(p);
		if (id >= 0) return id;
	}

	return -1;
}

var PropClusterGroup::getJSONData()
{
	var data = BaseItem::getJSONData();
	data.getDynamicObject()->setProperty("clusterManager", clusterManager.getJSONData());
	return data;
}

void PropClusterGroup::loadJSONDataInternal(var data)
{
	BaseItem::loadJSONDataInternal(data);
	clusterManager.loadJSONData(data.getProperty("clusterManager", var()));
}
