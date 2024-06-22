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
#include "Common/CommonIncludes.h"

class PropManager :
	public BaseManager<Prop>,
	public OSCReceiver::Listener<OSCReceiver::RealtimeCallback>,
	public Prop::PropListener,
	public ZeroconfManager::ZeroconfSearcher::SearcherListener,
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

	Factory<Prop> factory;

	const int localPort = 10000;

	Trigger* autoAssignIdTrigger;
	Trigger* assignPropIdTrigger;
	Trigger* clearAll;
	BoolParameter* disablePreview;

	ControllableContainer connectionCC;
	Trigger* detectProps;

	ControllableContainer controlsCC;
	Trigger* enableAll;
	Trigger* disableAll;
	Trigger* powerOffAll;
	Trigger* resetAll;
	FloatParameter* globalBrightness;

	ControllableContainer playbackCC;
	Trigger* uploadAll;
	BoolParameter* playbackMode;
	StringParameter* fileName;
	Trigger* loadAll;
	Trigger* playAll;
	Trigger* stopAll;
	Trigger* deleteAllPlayback;
	Trigger* deleteAllScripts;
	BoolParameter* loop;

	BoolParameter* autoAddNetworkProps;
	BoolParameter* autoAddUSBProps;
	ZeroconfManager::ZeroconfSearcher* zeroconfSearcher;

	//usb
	struct VidPid
	{
		int vid;
		int pid;
	};
	Array<VidPid> vidpids;
	Array<SerialDevice*> pendingDevices;

	void setupReceiver();

	Prop* createPropIfNotExist(const String& type, const String& host, const String& id, const String& name = "");
	Prop* getPropWithDeviceID(const String& deviceID);
	Prop* getPropWithId(int id, Prop* excludeProp = nullptr);

	void onControllableFeedbackUpdate(ControllableContainer* cc, Controllable* c) override;

	void addItemInternal(Prop* p, var) override;
	void addItemsInternal(Array<Prop*> props, var) override;
	void removeItemInternal(Prop* p) override;
	void removeItemsInternal(Array<Prop*> props) override;

	void clear() override;

	int getFirstAvailableID();
	void propIDChanged(Prop* p, int previousID) override;

	// Inherited via Listener
	virtual void oscMessageReceived(const OSCMessage& message) override;

	void serviceAdded(ZeroconfManager::ServiceInfo* s) override;
	void addPropForService(ZeroconfManager::ServiceInfo* s);

	void afterLoadJSONDataInternal() override;

	//USB connection
	void portAdded(SerialDeviceInfo* d) override;
	void portRemoved(SerialDeviceInfo* d) override;

	void checkSerialDevices();
	void checkDeviceDeviceID(SerialDeviceInfo* d);
	Prop* addPropForHardwareID(SerialDevice* d, String deviceID, String type);

	void serialDataReceived(SerialDevice* d, const var& data) override;
	void timerCallback(int timerID) override;
};