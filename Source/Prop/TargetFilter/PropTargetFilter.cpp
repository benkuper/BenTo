/*
  ==============================================================================

	PropTargetFilter.cpp
	Created: 5 Feb 2019 11:13:17am
	Author:  jonglissimo

  ==============================================================================
*/

#include "Prop/PropIncludes.h"

PropTargetFilter::PropTargetFilter(StringRef name) :
	BaseItem(name)
{
	isSelectable = false;
}

PropTargetFilter::~PropTargetFilter()
{
}

bool PropTargetFilter::isPropValid(Prop* p)
{
	return getTargetIDForProp(p) >= 0;
}

int PropTargetFilter::getTargetIDForProp(Prop* p)
{
	return -1;
}

PropFilterGlobalID::PropFilterGlobalID(var params) :
	PropTargetFilter(getTypeString())
{
	id = addIntParameter("ID", "Global ID to filter", 0);
}

PropFilterGlobalID::~PropFilterGlobalID()
{
}

int PropFilterGlobalID::getTargetIDForProp(Prop* p)
{
	return p->globalID->intValue() == id->intValue() ? id->intValue() : -1;
}


PropFilterPropShape::PropFilterPropShape(var params) :
	PropTargetFilter(getTypeString())
{
	shape = addEnumParameter("Type", "The type of prop to filter");
	Prop::fillTypeOptions(shape);
}

PropFilterPropShape::~PropFilterPropShape()
{
}

int PropFilterPropShape::getTargetIDForProp(Prop* p)
{
	return p->shape->getValueData() == shape->getValueData() ? p->globalID->intValue() : -1;
}

PropFilterCluster::PropFilterCluster(PropClusterGroupManager* manager) :
	PropTargetFilter(getTypeString()),
	manager(manager)
{
	jassert(manager != nullptr);

	useLocalID = addBoolParameter("Use Local ID", "If checked and applicable, will return local cluster ID instead of global", true);
	specificClusterGroup = addBoolParameter("Specific Group", "Search in specific cluster Group", false);
	clusterGroup = addTargetParameter("Family", "Family to filter", manager);
	clusterGroup->targetType = TargetParameter::CONTAINER;
	clusterGroup->maxDefaultSearchLevel = 0;
	clusterGroup->showParentNameInEditor = false;
	clusterGroup->setEnabled(false);

	specificCluster = addBoolParameter("Specific Cluster", "Search in specific cluster", false);
	cluster = addTargetParameter("Cluster", "Cluster to filter", manager);
	cluster->targetType = TargetParameter::CONTAINER;
	cluster->maxDefaultSearchLevel = 2;
	cluster->defaultContainerTypeCheckFunc = &PropFilterCluster::targetIsCluster;
	cluster->setEnabled(false);

	specificID = addBoolParameter("Specific Local ID", "Search for specific Local Cluster ID", false);
	id = addIntParameter("Local Prop ID", "The local cluster prop ID", 0);
	id->setEnabled(false);
}

PropFilterCluster::~PropFilterCluster()
{
}

int PropFilterCluster::getTargetIDForProp(Prop* p)
{
	int targetID = -1;

	if (specificClusterGroup->boolValue())
	{
		PropClusterGroup* cg = dynamic_cast<PropClusterGroup*>(clusterGroup->targetContainer.get());
		if (cg == nullptr) return -1;
		targetID = cg->getLocalPropID(p);
	}
	else if (specificCluster->boolValue())
	{
		PropCluster* c = dynamic_cast<PropCluster*>(cluster->targetContainer.get());
		if (c == nullptr) return -1;
		targetID = c->getLocalPropID(p);
	}
	else
	{
		for (auto& cg : manager->items)
		{
			int localID = cg->getLocalPropID(p);
			if (localID >= 0 && (!specificID->boolValue() || localID == id->intValue()))
			{
				targetID = localID;
				break;
			}
		}
	}

	if (specificID->boolValue() && targetID != id->intValue()) targetID = -1;

	if (targetID >= 0 && !useLocalID->boolValue()) targetID = p->globalID->intValue();
	return targetID;
}

void PropFilterCluster::onContainerParameterChangedInternal(Parameter* p)
{
	if (p == specificClusterGroup)
	{
		clusterGroup->setEnabled(specificClusterGroup->boolValue());
		if (specificClusterGroup->boolValue()) specificCluster->setValue(false);
	}
	else if (p == specificCluster)
	{
		cluster->setEnabled(specificCluster->boolValue());
		if (specificCluster->boolValue()) specificClusterGroup->setValue(false);
	}
	else if (p == specificID)
	{
		id->setEnabled(specificID->boolValue());
	}
}

bool PropFilterCluster::targetIsCluster(ControllableContainer* cc)
{
	return dynamic_cast<PropCluster*>(cc) != nullptr;
}





PropFilterScript::PropFilterScript(var params) :
	PropTargetFilter("Script")
{
	addChildControllableContainer(&script);
	script.userCanRemove = false;
	script.userCanDuplicate = false;
	script.canBeReorderedInEditor = false;
}

PropFilterScript::~PropFilterScript()
{
}

int PropFilterScript::getTargetIDForProp(Prop* p)
{
	Array<var> args;
	args.add(p->getScriptObject(), p->globalID->intValue());
	return script.callFunction("getID", args);
}

var PropFilterScript::getJSONData(bool includeNonOverriden)
{
	var data = PropTargetFilter::getJSONData(includeNonOverriden);
	data.getDynamicObject()->setProperty("script", script.getJSONData());
	return data;
}

void PropFilterScript::loadJSONDataItemInternal(var data)
{
	PropTargetFilter::loadJSONDataItemInternal(data);
	script.loadJSONData(data.getProperty("script", var()));
}
