/*
  ==============================================================================

    PropTargetFilter.cpp
    Created: 5 Feb 2019 11:13:17am
    Author:  jonglissimo

  ==============================================================================
*/

#include "PropTargetFilter.h"
#include "../PropManager.h"
#include "Prop/Cluster/PropClusterGroupManager.h"

PropTargetFilter::PropTargetFilter(StringRef name) :
	BaseItem(name)
{
	isSelectable = false;
}

PropTargetFilter::~PropTargetFilter()
{
}

bool PropTargetFilter::isPropValid(Prop * p)
{
	return getTargetIDForProp(p) >= 0;
}

int PropTargetFilter::getTargetIDForProp(Prop * p)
{
	return -1;
}




PropFilterGlobalID::PropFilterGlobalID() :
	PropTargetFilter("Global ID Filter")
{
	id = addIntParameter("ID", "Global ID to filter", 0);
}

PropFilterGlobalID::~PropFilterGlobalID()
{
}

int PropFilterGlobalID::getTargetIDForProp(Prop * p)
{
	return p->globalID->intValue() == id->intValue() ? id->intValue() : -1;
}


PropFilterPropFamily::PropFilterPropFamily() :
	PropTargetFilter("Family Filter")
{
	family = addTargetParameter("Family", "Family to filter", &PropManager::getInstance()->familiesCC);
	family->targetType = TargetParameter::CONTAINER;
}

PropFilterPropFamily::~PropFilterPropFamily()
{
}

int PropFilterPropFamily::getTargetIDForProp(Prop * p)
{
	return p->family == family->targetContainer ? p->globalID->intValue() : -1;
}

PropFilterPropType::PropFilterPropType() :
	PropTargetFilter("Type Filter")
{
	type = addEnumParameter("Type", "The type of prop to filter");
	Prop::fillTypeOptions(type);
}

PropFilterPropType::~PropFilterPropType()
{
}

int PropFilterPropType::getTargetIDForProp(Prop * p)
{
	return p->type->getValueData() == type->getValueData() ? p->globalID->intValue() : -1;
}

PropFilterCluster::PropFilterCluster() :
	PropTargetFilter("Cluster Filter")
{
	specificClusterGroup = addBoolParameter("Specific Cluster", "Search in specific cluster Group", false);
	clusterGroup = addTargetParameter("Family", "Family to filter", PropClusterGroupManager::getInstance());
	clusterGroup->targetType = TargetParameter::CONTAINER;
	clusterGroup->maxDefaultSearchLevel = 1;

	specificCluster = addBoolParameter("Specific Cluster", "Search in specific cluster", PropClusterGroupManager::getInstance());
	cluster = addTargetParameter("Cluster", "Cluster to filter");
	cluster->targetType = TargetParameter::CONTAINER;
	cluster->maxDefaultSearchLevel = 2;
	cluster->setEnabled(false);

	specificID = addBoolParameter("Specific Local ID", "Search for specific Local Cluster ID", false);
	id = addIntParameter("Local Prop ID", "The local cluster prop ID",0);
}

PropFilterCluster::~PropFilterCluster()
{
}

int PropFilterCluster::getTargetIDForProp(Prop * p)
{
	int targetID = -1;

	if (specificClusterGroup->boolValue())
	{
		PropClusterGroup * cg = dynamic_cast<PropClusterGroup *>(clusterGroup->targetContainer.get());
		if (cg == nullptr) return -1;
		targetID = cg->getLocalPropID(p);
	}
	else if (specificCluster->boolValue())
	{
		PropCluster * c = dynamic_cast<PropCluster *>(cluster->targetContainer.get());
		if (c == nullptr) return -1;
		targetID = c->getLocalPropID(p);
	}
	else
	{
		for (auto &cg : PropClusterGroupManager::getInstance()->items)
		{
			targetID = cg->getLocalPropID(p);
			if (targetID >= 0) break;
		}
	}

	if (specificID->boolValue()) return targetID == id->intValue() ? targetID : -1;
	return targetID;
}





PropFilterScript::PropFilterScript() :
	PropTargetFilter("Script Filter")
{
	addChildControllableContainer(&script);
}

PropFilterScript::~PropFilterScript()
{
}

int PropFilterScript::getTargetIDForProp(Prop * p)
{
	Array<var> args;
	args.add(p->getScriptObject());
	return script.callFunction("getID", args);
}
