/*
  ==============================================================================

    LightBlockModelParameterEditor.h
    Created: 10 Apr 2018 8:01:56pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "../LightBlockModelParameter.h"

class LightBlockModelParameterEditor :
	public BaseItemEditor
{
public:
	LightBlockModelParameterEditor(LightBlockModelParameter * a, bool isRoot);
	~LightBlockModelParameterEditor();

	LightBlockModelParameter * arg;

	std::unique_ptr<BoolToggleUI> editableUI;

	void resetAndBuild() override;
	void resizedInternalHeaderItemInternal(Rectangle<int> &r) override;

};
