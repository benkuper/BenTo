/*
  ==============================================================================

	LightBlockColorProviderEditor.h
	Created: 12 Oct 2023 11:43:57am
	Author:  bkupe

  ==============================================================================
*/

#pragma once

class LightBlockColorProviderEditor :
	public BaseItemEditor
{
public:
	LightBlockColorProviderEditor(Array<LightBlockColorProvider*> p, bool isRoot = false);
	~LightBlockColorProviderEditor();

	LightBlockColorProvider* provider;

	std::unique_ptr<TriggerUI> assignUI;

	void resizedInternalHeaderItemInternal(Rectangle<int>& r) override;
};