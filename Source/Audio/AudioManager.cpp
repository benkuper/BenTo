/*
  ==============================================================================

	AudioManager.cpp
	Created: 27 Apr 2018 12:03:27pm
	Author:  Ben

  ==============================================================================
*/

#include "AudioManager.h"
#include "ui/AudioManagerHardwareEditor.h"

juce_ImplementSingleton(AudioManager)

AudioManager::AudioManager() :
	ControllableContainer("Audio Manager"),
	hs(&am)
{
	editorIsCollapsed = true; 
	
	am.initialiseWithDefaultDevices(2, 2);
	am.addAudioCallback(&player);

	graph.reset();

	AudioDeviceManager::AudioDeviceSetup setup;
	am.getAudioDeviceSetup(setup);

	graph.setPlayConfigDetails(0, 2, setup.sampleRate, setup.bufferSize);
	graph.prepareToPlay(setup.sampleRate, setup.bufferSize);

	//graph.addNode(new AudioProcessorGraph::AudioGraphIOProcessor(AudioProcessorGraph::AudioGraphIOProcessor::audioInputNode), 1);
	graph.addNode(new AudioProcessorGraph::AudioGraphIOProcessor(AudioProcessorGraph::AudioGraphIOProcessor::audioOutputNode), AudioProcessorGraph::NodeID(2));
	player.setProcessor(&graph);

	addChildControllableContainer(&hs);
}


AudioManager::~AudioManager()
{
	graph.clear();

	am.removeAudioCallback(&player);
	player.setProcessor(nullptr);
}

AudioModuleHardwareSettings::AudioModuleHardwareSettings(AudioDeviceManager * am) :
	ControllableContainer("Hardware Settings"),
	am(am)
{
	hideEditorHeader = true;
	
}

InspectableEditor * AudioModuleHardwareSettings::getEditor(bool isRoot)
{
	return new AudioManagerHardwareEditor(this, isRoot);
}
