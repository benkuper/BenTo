/*
  ==============================================================================

    LightBlockModelGroup.cpp
    Created: 13 Apr 2018 2:03:02am
    Author:  Ben

  ==============================================================================
*/

var LightBlockModelGroup::getJSONData(bool includeNonOverriden)
{
	var data = ControllableContainer::getJSONData(includeNonOverriden);
	for (auto &cc : controllableContainers)
	{
		var ccData = cc->getJSONData();
		if(ccData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty(cc->shortName, ccData);
	}
	return data;
}

void LightBlockModelGroup::loadJSONDataInternal(var data)
{
	ControllableContainer::loadJSONDataInternal(data);
	for (auto &cc : controllableContainers)
	{
		cc->loadJSONData(data.getProperty(cc->shortName, var()));
	}
}
