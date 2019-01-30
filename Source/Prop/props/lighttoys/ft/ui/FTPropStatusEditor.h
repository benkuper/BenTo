/*
  ==============================================================================

    FTPropStatusEditor.h
    Created: 30 Jan 2019 5:46:49pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "../FTPropStatus.h"

class FTPropStatusEditor :
	public InspectableEditor
{
public:
	FTPropStatusEditor(FTPropStatus * ftp, bool isRoot);
	~FTPropStatusEditor();

	ScopedPointer<BoolToggleUI> isConnectedUI;
	ScopedPointer<FloatSliderUI> voltageUI;
	ScopedPointer<TriggerButtonUI> flashUI;

	void resized() override;
};