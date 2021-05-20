/*
  ==============================================================================

    LightBlockModelParameterEditor.cpp
    Created: 10 Apr 2018 8:01:56pm
    Author:  Ben

  ==============================================================================
*/

LightBlockModelParameterEditor::LightBlockModelParameterEditor(LightBlockModelParameter * a, bool isRoot) :
	BaseItemEditor(a, isRoot),
	arg(a)
{
	editableUI.reset(arg->editable->createToggle());

	addAndMakeVisible(editableUI.get());

	resetAndBuild();
}

LightBlockModelParameterEditor::~LightBlockModelParameterEditor()
{
}

void LightBlockModelParameterEditor::resetAndBuild()
{
	BaseItemEditor::resetAndBuild();
	for (auto &cui : childEditors)
	{
		ControllableEditor * ce = dynamic_cast<ControllableEditor *>(cui);
		if (ce != nullptr) ce->setShowLabel(false);
	}
}


void LightBlockModelParameterEditor::resizedInternalHeaderItemInternal(Rectangle<int>& r)
{
	editableUI->setBounds(r.removeFromRight(70).reduced(2));
	r.removeFromRight(2);
}
