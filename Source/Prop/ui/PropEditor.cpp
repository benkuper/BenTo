/*
  ==============================================================================

    PropEditor.cpp
    Created: 12 May 2020 7:17:02pm
    Author:  bkupe

  ==============================================================================
*/

PropEditor::PropEditor(Prop* p, bool isRoot) :
    BaseItemEditor(p, isRoot),
    prop(p)
{
    logInUI.reset(prop->logIncoming->createToggle());
    logOutUI.reset(prop->logOutgoing->createToggle());
    logInUI->showLabel = true;
    logOutUI->showLabel = true;
    addAndMakeVisible(logInUI.get());
    addAndMakeVisible(logOutUI.get());
}

PropEditor::~PropEditor()
{
}

void PropEditor::resizedInternalHeaderItemInternal(Rectangle<int>& r)
{
    logOutUI->setBounds(r.removeFromRight(90).reduced(2));
    r.removeFromRight(10);
    logInUI->setBounds(r.removeFromRight(90).reduced(2));
}
