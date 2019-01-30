/*
  ==============================================================================

    LighttoysVisualProp.h
    Created: 30 Jan 2019 10:04:30am
    Author:  bkupe

  ==============================================================================
*/

#pragma once

#include "../../../Prop.h"

class LighttoysVisualProp :
	public Prop
{
public:
	LighttoysVisualProp(var params);
	~LighttoysVisualProp();

	String getTypeString() const override { return "Lighttoys Visual"; }

	virtual void sendColorsToPropInternal() override;

	static void autoDetectRemotes();

	static LighttoysVisualProp * create(var params) { return new LighttoysVisualProp(params); }
};