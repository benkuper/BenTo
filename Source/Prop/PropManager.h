/*
  ==============================================================================

    PropManager.h
    Created: 10 Apr 2018 6:59:29pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "Prop.h"

class PropManager :
	public BaseManager<Prop>,
	public OSCReceiver::Listener<OSCReceiver::RealtimeCallback>
{
public:
	juce_DeclareSingleton(PropManager, true)
	
	PropManager();
	~PropManager();

	OSCSender sender;
	OSCReceiver receiver;

	Factory<Prop> factory;
	
	const int localPort = 10000;

	StringParameter * localHost;
	StringParameter * remoteHost;

	Trigger * detectProps;
	Trigger * autoAssignIdTrigger;

	void setupReceiver();

	Prop * getPropWithId(const String &id);

	void onContainerParameterChanged(Parameter * p) override;
	void onContainerTriggerTriggered(Trigger * t) override;

	// Inherited via Listener
	virtual void oscMessageReceived(const OSCMessage & message) override;
};