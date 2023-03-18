/*
  ==============================================================================

    AudioManagerEditor.h
    Created: 27 Apr 2018 12:04:23pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "../AudioManager.h"

class AudioManagerHardwareEditor :
	public GenericControllableContainerEditor
{
public:
	AudioManagerHardwareEditor(AudioManagerHardwareSettings * hs, bool isRoot);
	~AudioManagerHardwareEditor();

	AudioManagerHardwareSettings * hs;
	AudioDeviceSelectorComponent selector;

	void setCollapsed(bool value, bool force = false, bool animate = true, bool doNotRebuild = false) override;
	void resizedInternalContent(Rectangle<int> &r) override;
};