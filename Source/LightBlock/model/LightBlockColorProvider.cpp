/*
  ==============================================================================

	LightBlockColorProvider.cpp
	Created: 12 Apr 2018 2:10:56pm
	Author:  Ben

  ==============================================================================
*/

LightBlockColorProvider::LightBlockColorProvider(const String & name, bool canBeDisabled, bool canHaveScripts, ProviderType providerType) :
	BaseItem(name, canBeDisabled, canHaveScripts),
	providerType(providerType)
{
	assignToAll = addTrigger("Assign To All", "Assign this Model or Preset to all props");
	viewUISize->isSavable = false;
}

LightBlockColorProvider::~LightBlockColorProvider() {
	LightBlockColorProvider::masterReference.clear();
}


BakeData LightBlockColorProvider::getBakeDataForProp(Prop *)
{
	return BakeData();
}

void LightBlockColorProvider::onContainerTriggerTriggered(Trigger * t)
{
	BaseItem::onContainerTriggerTriggered(t);
	if (t == assignToAll)
	{
		for (auto & p : PropManager::getInstance()->items)
		{
			p->activeProvider->setValueFromTarget(this);
		}
	}

}

Array<Colour> LightBlockColorProvider::getColors(Prop* p, double time, var params)
{
	return Array<Colour>();
}

