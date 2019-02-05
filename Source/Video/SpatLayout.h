/*
  ==============================================================================

    SpatLayout.h
    Created: 5 Feb 2019 7:31:37pm
    Author:  jonglissimo

  ==============================================================================
*/

#pragma once

#include "SpatItem.h"

class SpatLayout :
	public BaseItem
{
public:
	SpatLayout();
	~SpatLayout();

	BaseManager<SpatItem> spatItemManager;

	String getTypeString() const override { return "Layout"; }

	var getJSONData() override;
	void loadJSONDataInternal(var data) override;
};