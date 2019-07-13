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

	std::unique_ptr<BoolToggleUI> isConnectedUI;
	std::unique_ptr<FloatSliderUI> voltageUI;
	std::unique_ptr<TriggerButtonUI> flashUI;

	void resized() override;
};