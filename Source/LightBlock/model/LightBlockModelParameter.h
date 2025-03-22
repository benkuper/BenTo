/*
  ==============================================================================

    LightBlockModelParameter.h
    Created: 10 Apr 2018 7:49:54pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

class LightBlockModelParameter :
	public BaseItem
{
public:
	LightBlockModelParameter(const String &name = "arg", Parameter * p = nullptr);
	Parameter * param;
	//StringParameter * argumentName; 
	BoolParameter * editable;

	InspectableEditor * getEditorInternal(bool isRoot, Array<Inspectable*> inspectables = {}) override;

	var getJSONData(bool includeNonOverriden = false) override;
	void loadJSONDataInternal(var data) override;

	void onContainerNiceNameChanged() override;

	virtual String getTypeString() const override { return param->getTypeString(); }

};
