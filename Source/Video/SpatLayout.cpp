/*
  ==============================================================================

    SpatLayout.cpp
    Created: 5 Feb 2019 7:31:37pm
    Author:  jonglissimo

  ==============================================================================
*/

SpatLayout::SpatLayout() :
	BaseItem(getTypeString(), false),
	spatItemManager("Items")
{
	addChildControllableContainer(&spatItemManager);
	spatItemManager.hideInEditor = true;
}

SpatLayout::~SpatLayout()
{
}

var SpatLayout::getJSONData()
{
	var data = BaseItem::getJSONData();
	data.getDynamicObject()->setProperty("itemManager", spatItemManager.getJSONData());
	return data;
}

void SpatLayout::loadJSONDataInternal(var data)
{
	BaseItem::loadJSONDataInternal(data);
	spatItemManager.loadJSONData(data.getProperty("itemManager", var()));
}
