/*
  ==============================================================================

    AudioManager.h
    Created: 27 Apr 2018 12:03:27pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"

#define AUDIO_INPUT_GRAPH_ID AudioProcessorGraph::NodeID(1)
#define AUDIO_OUTPUT_GRAPH_ID AudioProcessorGraph::NodeID(2)

class AudioManagerHardwareSettings :
	public ControllableContainer
{
public:
	AudioManagerHardwareSettings(AudioDeviceManager * am);
	~AudioManagerHardwareSettings() {}
	AudioDeviceManager * am;

	InspectableEditor * getEditorInternal(bool isRoot, Array<Inspectable*> inspectables = {}) override;
};

class AudioManager :
	public ControllableContainer,
	public AudioIODeviceCallback
{
public:
	juce_DeclareSingleton(AudioManager, true);

	AudioManager();
	~AudioManager();

	AudioManagerHardwareSettings hs;
	AudioDeviceManager am;
	AudioProcessorPlayer player;
	AudioProcessorGraph graph;

	// Inherited via AudioIODeviceCallback
	virtual void audioDeviceIOCallbackWithContext(const float* const* inputChannelData,
		int numInputChannels,
		float* const* outputChannelData,
		int numOutputChannels,
		int numSamples,
		const AudioIODeviceCallbackContext& context) override;

	virtual void audioDeviceAboutToStart(AudioIODevice* device) override;
	virtual void audioDeviceStopped() override;

	var getJSONData() override;
	void loadJSONDataInternal(var data) override;
};
