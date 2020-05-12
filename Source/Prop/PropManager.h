/*
  ==============================================================================

    PropManager.h
    Created: 10 Apr 2018 6:59:29pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "Prop.h"
#include "PropFamily.h"
#include "Common/Zeroconf/ZeroconfManager.h"

class PropManager :
	public BaseManager<Prop>,
	public OSCReceiver::Listener<OSCReceiver::RealtimeCallback>,
	public Prop::PropListener,
	public ZeroconfManager::ZeroconfSearcher::SearcherListener,
	public Thread,
	public URL::DownloadTask::Listener
{
public:
	juce_DeclareSingleton(PropManager, true)
	
	PropManager();
	~PropManager();

	OSCSender sender;
	OSCReceiver receiver;

	ControllableContainer familiesCC;
	Array<PropFamily *> families;
	Factory<Prop> factory;
	
	const int localPort = 10000;

	Trigger * autoAssignIdTrigger;
	Trigger* clearAll;

	ControllableContainer connectionCC;
	Trigger* detectProps;

	ControllableContainer controlsCC;
	BoolParameter* sendFeedback;
	Trigger* powerOffAll;
	Trigger* resetAll;

	ControllableContainer showCC;
	Trigger* bakeAll;
	BoolParameter* bakeMode;
	StringParameter* fileName;
	Trigger* loadAll;
	Trigger* playAll;
	Trigger* stopAll;
	BoolParameter* loop;

	BoolParameter* autoAddProps;
	ZeroconfManager::ZeroconfSearcher* zeroconfSearcher;

	std::unique_ptr<URL::DownloadTask> propDownloadTask;

	void setupReceiver();

	Prop* createPropIfNotExist(const String& type, const String& host, const String& id);
	Prop * getPropWithHardwareId(const String &hardwareId);
	Prop * getPropWithId(int id, Prop * excludeProp = nullptr);

	PropFamily * getFamilyWithName(StringRef familyName);

	void onControllableFeedbackUpdate(ControllableContainer * cc, Controllable *c) override;

	void addItemInternal(Prop * p, var) override;
	void removeItemInternal(Prop * p) override;

	void clear() override;

	int getFirstAvailableID();
	void propIDChanged(Prop * p, int previousID) override;

	// Inherited via Listener
	virtual void oscMessageReceived(const OSCMessage & message) override;

	void serviceAdded(ZeroconfManager::ServiceInfo * s) override;

	void updatePropsAndFamiliesDefinitions();

	void run() override;

	// Inherited via Listener
	virtual void finished(URL::DownloadTask* task, bool success) override;
};