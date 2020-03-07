/*
  ==============================================================================

    OpenProp.h
    Created: 7 Mar 2020 4:58:39pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once

#include "../../Prop.h"

class OpenProp :
	public Prop
{
public:
	OpenProp(StringRef name = "Open", var params = var());
	~OpenProp();

	Script script;

	void sendColorsToPropInternal() override;

	var getJSONData() override;
	void loadJSONDataItemInternal(var data) override;
};