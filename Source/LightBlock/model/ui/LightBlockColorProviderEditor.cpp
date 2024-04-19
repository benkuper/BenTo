/*
  ==============================================================================

	LightBlockColorProviderEditor.cpp
	Created: 12 Oct 2023 11:43:57am
	Author:  bkupe

  ==============================================================================
*/

#include "LightBlock/LightBlockIncludes.h"


LightBlockColorProviderEditor::LightBlockColorProviderEditor(Array<LightBlockColorProvider*> providers, bool isRoot) :
	BaseItemEditor(Inspectable::getArrayAs<LightBlockColorProvider, BaseItem>(providers), isRoot),
	provider(providers[0])
{
	assignUI.reset(provider->assignToAll->createButtonUI());
	addAndMakeVisible(assignUI.get());
}

LightBlockColorProviderEditor::~LightBlockColorProviderEditor()
{
}

void LightBlockColorProviderEditor::resizedInternalHeaderItemInternal(Rectangle<int>& r)
{
	assignUI->setBounds(r.removeFromRight(120).reduced(2));
	r.removeFromRight(4);
}
