/*
  ==============================================================================

    SpatLayout.h
    Created: 5 Feb 2019 7:31:37pm
    Author:  jonglissimo

  ==============================================================================
*/

#pragma once

class SpatLayout :
	public BaseItem
{
public:
	SpatLayout();
	~SpatLayout();

	Manager<SpatItem> spatItemManager;

	String getTypeString() const override { return "Layout"; }

	var getJSONData(bool includeNonOverriden = false) override;
	void loadJSONDataInternal(var data) override;
};