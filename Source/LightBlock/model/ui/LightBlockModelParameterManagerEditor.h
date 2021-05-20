/*
  ==============================================================================

    LightBlockModelParameterManagerEditor.h
    Created: 10 Apr 2018 7:55:28pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

class LightBlockModelParameterManagerEditor :
	public GenericManagerEditor<LightBlockModelParameter>
{
public:
	LightBlockModelParameterManagerEditor(LightBlockModelParameterManager * manager, bool isRoot);
	~LightBlockModelParameterManagerEditor() {};

	LightBlockModelParameterManager * pManager;

	void showMenuAndAddItem(bool) override;
};

