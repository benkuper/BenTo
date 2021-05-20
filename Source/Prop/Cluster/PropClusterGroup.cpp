/*
  ==============================================================================

    PropClusterGroup.cpp
    Created: 5 Feb 2019 11:12:42am
    Author:  jonglissimo

  ==============================================================================
*/

PropClusterGroup::PropClusterGroup() :
	BaseItem("Cluster Group"),
	clusterManager("Clusters")
{
	sendFeedback = addBoolParameter("Send Feedback", "If checked, Prop changes will be sent using their local ID", false);

	//clusterManager.hideEditorHeader = true;
	//clusterManager.skipControllableNameInAddress = true;
	clusterManager.selectItemWhenCreated = false;

	if (!Engine::mainEngine->isLoadingFile)
	{
		clusterManager.addItem(); //add one item here
	}

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

int PropClusterGroup::getClusterIDForProp(Prop* p)
{
	int index = 0;
	for (auto& c : clusterManager.items)
	{
		if (c->isPropIsInCluster(p)) return index;
		index++;
	}

	return -1;
}

Array<Colour> PropClusterGroup::getColorsForProp(Prop* p)
{
	int numPixels = p->resolution->intValue();
	Array<Colour> result;
	result.resize(numPixels);
	result.fill(Colours::transparentBlack);

	int id = -1;
	PropCluster* c = getClusterForProp(p, id);
	if (c != nullptr)
	{
		for (int i = 0; i < 3 && i < numPixels / 2; i++) result.set(numPixels-1-i, c->color->getColor());
		for (int i = numPixels - 1; i > numPixels - 1 - id -1 && i > numPixels / 2; i--) result.set(numPixels-1-i, Colour::fromHSV(id * 1.0f / 4, 1, 1, 1));
	}

	return result;
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
