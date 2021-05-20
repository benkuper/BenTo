/*
  ==============================================================================

	PropTargetFilterManager.cpp
	Created: 5 Feb 2019 12:43:58pm
	Author:  jonglissimo

  ==============================================================================
*/

PropTargetFilterManager::PropTargetFilterManager(PropClusterGroupManager* clusterGroupManager) :
	BaseManager<PropTargetFilter>("Prop Filters"),
	clusterGroupManager(clusterGroupManager)
{
	operationMode = addEnumParameter("Operation Mode", "Logic operation to apply");
	operationMode->addOption("OR", 0)->addOption("AND", 1);

	selectItemWhenCreated = false;
	managerFactory = &factory;
	factory.defs.add(Factory<PropTargetFilter>::Definition::createDef("", "Global ID", PropFilterGlobalID::create));
	factory.defs.add(Factory<PropTargetFilter>::Definition::createDef("", "Family", PropFilterPropFamily::create));
	factory.defs.add(Factory<PropTargetFilter>::Definition::createDef("", "Type", PropFilterPropType::create));
	if (clusterGroupManager != nullptr) factory.defs.add(new PropFilterCluster::PropFilterClusterDefinition("", "Cluster", PropFilterCluster::create, clusterGroupManager));
	factory.defs.add(Factory<PropTargetFilter>::Definition::createDef("", "Script", PropFilterScript::create));
}

PropTargetFilterManager::~PropTargetFilterManager()
{

}

int PropTargetFilterManager::getTargetIDForProp(Prop* p)
{
	int mode = operationMode->getValueData();
	int numProcessed = 0;

	int result = -1;

	for (auto& f : items)
	{
		if (!f->enabled->boolValue()) continue;

		int fID = f->getTargetIDForProp(p);

		if (fID >= 0)
		{
			if (mode == 0) return fID;
			else result = fID;
		}
		else
		{
			if (mode == 1) return -1;
		}

		numProcessed++;
	}

	if (numProcessed == 0) return p->globalID->intValue();
	return result;
}

void PropTargetFilterManager::addItemInternal(PropTargetFilter*, var)
{
	filerManagerListeners.call(&FilterManagerListener::filtersChanged);
}

void PropTargetFilterManager::removeItemInternal(PropTargetFilter*)
{
	filerManagerListeners.call(&FilterManagerListener::filtersChanged);
}

void PropTargetFilterManager::onControllableFeedbackUpdate(ControllableContainer*, Controllable* c)
{
	filerManagerListeners.call(&FilterManagerListener::filtersChanged);
}
