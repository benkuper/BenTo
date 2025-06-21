/*
  ==============================================================================

    LightBlockModelParameterManager.h
    Created: 10 Apr 2018 7:49:47pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

class LightBlockModelParameterManager :
	public Manager<LightBlockModelParameter>
{
public:
	LightBlockModelParameterManager();
	~LightBlockModelParameterManager() {}

	LightBlockModelParameter * addItemWithParam(Parameter * p, var data = var(), bool fromUndoableAction = false);
	LightBlockModelParameter * addItemFromType(Parameter::Type type, var data = var(), bool fromUndoableAction = false);

	LightBlockModelParameter * addItemFromData(var data, bool fromUndoableAction = false) override;

	void autoRenameItems();
	void removeItemInternal(LightBlockModelParameter * i) override;
	InspectableEditor * getEditorInternal(bool isRoot, Array<Inspectable*> inspectables = {}) override;

};
