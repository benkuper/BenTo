/*
  ==============================================================================

    LightBlockModelParameter.h
    Created: 10 Apr 2018 7:49:54pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"

class LightBlockModelParameter :
	public BaseItem
{
public:
	LightBlockModelParameter(const String &name = "arg", Parameter * p = nullptr);
	Parameter * param;
	//StringParameter * argumentName; 
	BoolParameter * editable;

	InspectableEditor * getEditor(bool isRoot) override;

	var getJSONData() override;
	void loadJSONDataInternal(var data) override;

	void onContainerNiceNameChanged() override;

	virtual String getTypeString() const override { return param->getTypeString(); }

};
