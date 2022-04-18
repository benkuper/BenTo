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
#include "Common/Serial/SerialManager.h"

class PropManager :
	public BaseManager<Prop>,
	public OSCReceiver::Listener<OSCReceiver::RealtimeCallback>,
	public Prop::PropListener,
	public ZeroconfManager::ZeroconfSearcher::SearcherListener,
	public Thread,
	public URL::DownloadTask::Listener,
	public SerialManager::SerialManagerListener,
	public SerialDevice::SerialDeviceListener,
	public MultiTimer
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
	BoolParameter* disablePreview;

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

	BoolParameter* autoAddNetworkProps;
	BoolParameter* autoAddUSBProps;
	ZeroconfManager::ZeroconfSearcher* zeroconfSearcher;

	std::unique_ptr<URL::DownloadTask> propDownloadTask;


	//usb
	struct VidPid
	{
		int vid;
		int pid;
	};
	Array<VidPid> vidpids;
	Array<SerialDevice*> pendingDevices;

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

	void afterLoadJSONDataInternal() override;

	void run() override;

	// Inherited via Listener
	virtual void finished(URL::DownloadTask* task, bool success) override;

	//USB connection
	void portAdded(SerialDeviceInfo* d) override;
	void portRemoved(SerialDeviceInfo* d) override;

	void checkSerialDevices();
	void checkDeviceHardwareID(SerialDeviceInfo* d);
	Prop* addPropForHardwareID(SerialDevice* d, String firmware, String type);

	void serialDataReceived(SerialDevice* d, const var& data) override;
	void timerCallback(int timerID) override;
};