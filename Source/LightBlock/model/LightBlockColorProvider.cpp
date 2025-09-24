/*
  ==============================================================================

	LightBlockColorProvider.cpp
	Created: 12 Apr 2018 2:10:56pm
	Author:  Ben

  ==============================================================================
*/

#include "LightBlock/LightBlockIncludes.h"
#include "Prop/PropIncludes.h"

LightBlockColorProvider::LightBlockColorProvider(const String& name, bool canBeDisabled, bool canHaveScripts, ProviderType providerType) :
	BaseItem(name, canBeDisabled, canHaveScripts),
	providerType(providerType)
{

	viewUISize->isSavable = false;
}

LightBlockColorProvider::~LightBlockColorProvider() {
	LightBlockColorProvider::masterReference.clear();
}


PlaybackData LightBlockColorProvider::getPlaybackDataForProp(Prop*)
{
	return PlaybackData();
}


InspectableEditor* LightBlockColorProvider::getEditorInternal(bool isRoot, Array<Inspectable*> inspectables)
{
	return new LightBlockColorProviderEditor(this, isRoot);
}

Array<Colour> LightBlockColorProvider::getColors(Prop* p, double time, var params)
{
	return Array<Colour>();
}

