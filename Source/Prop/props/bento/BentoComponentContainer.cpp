/*
  ==============================================================================

	BentoComponentContainer.cpp
	Created: 14 Oct 2023 12:42:45pm
	Author:  bkupe

  ==============================================================================
*/

#include "Prop/PropIncludes.h"

BentoComponentContainer::BentoComponentContainer(BentoProp* prop) :
	ControllableContainer("Components"),
	Thread("Bento OSCQuery"),
	prop(prop),
	wsPort(-1),
	isUpdatingStructure(false)
{
	startThread();
}

BentoComponentContainer::~BentoComponentContainer()
{
	stopThread(2000);
	if (wsClient != nullptr) wsClient->stop();
}

void BentoComponentContainer::setupWSClient()
{
	prop->isConnected->setValue(false);
	if (wsClient != nullptr) wsClient->stop();
	wsClient.reset();
	if (isCurrentlyLoadingData) return;

	if (!prop->enabled->boolValue()) return;
	wsClient.reset(new SimpleWebSocketClient());
	wsClient->addWebSocketListener(this);

	String host = prop->remoteHost->stringValue();
	String url = host;
	if (wsPort != -1) url += ":" + String(wsPort) + "/";
	DBG("Setting up client at " << url);
	wsClient->start(url);
}

void BentoComponentContainer::closeWSClient()
{
	if (wsClient == nullptr) return;
	wsClient->stop();

	stopThread(1000);
}


void BentoComponentContainer::onControllableFeedbackUpdate(ControllableContainer* cc, Controllable* c)
{

	if (!prop->enabled->boolValue()) return;
	if (isUpdatingStructure) return;
	if (isCurrentlyLoadingData) return;
	if (noFeedbackList.contains(c)) return;

	OSCMessage m = OSCHelpers::getOSCMessageForControllable(c, this);
	prop->sendMessageToProp(m);

}

void BentoComponentContainer::onContainerParameterChanged(Parameter* p)
{
	if (!prop->enabled->boolValue()) return;
	if (isUpdatingStructure) return;
	if (isCurrentlyLoadingData) return;
	if (noFeedbackList.contains(p)) return;

	OSCMessage m = OSCHelpers::getOSCMessageForControllable(p, this);
	m.setAddressPattern("/root" + m.getAddressPattern().toString());
	prop->sendMessageToProp(m);
}

void BentoComponentContainer::onContainerTriggerTriggered(Trigger* t)
{
	if (!prop->enabled->boolValue()) return;
	if (isUpdatingStructure) return;
	if (isCurrentlyLoadingData) return;
	if (noFeedbackList.contains(t)) return;

	OSCMessage m = OSCHelpers::getOSCMessageForControllable(t, this);
	m.setAddressPattern("/root" + m.getAddressPattern().toString());
	prop->sendMessageToProp(m);
}


void BentoComponentContainer::syncData(bool force)
{
	if (isCurrentlyLoadingData || Engine::mainEngine->isLoadingFile) return;
	if (!force && prop->isConnected->boolValue()) return;
	startThread();
}

void BentoComponentContainer::updateTreeFromData(var data)
{
	if (data.isVoid()) return;

	isUpdatingStructure = true;

	Array<String> enableListenContainers;
	Array<String> expandedContainers;
	Array<WeakReference<ControllableContainer>> containers = getAllContainers(true);


	OSCQueryHelpers::updateContainerFromData(this, data, false, [this](const String& ccName) { return new BentoSubComponent(ccName, this); });

	isUpdatingStructure = false;

	prop->updateUniverses();
}

void BentoComponentContainer::connectionOpened()
{
	NLOG(niceName, "Websocket connection is opened, let's get bi, baby !");
	prop->isConnected->setValue(true);
}

void BentoComponentContainer::connectionClosed(int status, const String& reason)
{
	NLOG(niceName, "Websocket connection is closed, bye bye!");
	prop->isConnected->setValue(false);
}

void BentoComponentContainer::connectionError(const String& errorMessage)
{
	NLOGERROR(niceName, "Connection error " << errorMessage);
	prop->isConnected->setValue(false);
}

void BentoComponentContainer::dataReceived(const MemoryBlock& data)
{
	if (!prop->enabled->boolValue()) return;

	OSCPacketParser parser(data.getData(), (int)data.getSize());
	OSCMessage m = parser.readMessage();

	if (m.getAddressPattern().toString() == "/bye")
	{
		NLOG(niceName, "Prop is saying bye, prop will : " << (m.size() > 0 ? m[0].getString() : "[notset]"));

		prop->isConnected->setValue(false);
		return;
	}

	if (Controllable* c = OSCHelpers::findControllable(this, m))
	{
		noFeedbackList.add(c);
		OSCHelpers::handleControllableForOSCMessage(c, m);
		noFeedbackList.removeAllInstancesOf(c);
	}
}

void BentoComponentContainer::messageReceived(const String& message)
{
	if (!prop->enabled->boolValue()) return;

}

void BentoComponentContainer::run()
{
	wait(100); //safety
	requestHostInfo();
}

void BentoComponentContainer::requestHostInfo()
{
	if (prop->remoteHost == nullptr || prop->remoteHost->stringValue().isEmpty()) return

		prop->isConnected->setValue(false);

	URL url("http://" + prop->remoteHost->stringValue() + "/?HOST_INFO");
	StringPairArray responseHeaders;
	int statusCode = 0;

	std::unique_ptr<InputStream> stream(url.createInputStream(
		URL::InputStreamOptions(URL::ParameterHandling::inAddress)
		.withConnectionTimeoutMs(1000)
		.withResponseHeaders(&responseHeaders)
		.withStatusCode(&statusCode)
		.withProgressCallback([this](int, int) { LOG("check here"); return !isClearing && !Engine::mainEngine->isClearing && !threadShouldExit(); })
	));

	if (threadShouldExit()) return;

	bool success = false;

#if JUCE_WINDOWS
	if (statusCode != 200)
	{
		stream.reset();
	}
#endif

	if (stream != nullptr)
	{
		String content = stream->readEntireStreamAsString();

		var data = JSON::parse(content);
		if (data.isObject())
		{
			//NLOG(niceName, "Received HOST_INFO :\n" << JSON::toString(data));
			wsPort = data.getProperty("WS_PORT", -1);
			success = true;

			requestStructure();
			setupWSClient();
		}

	}

	if (!success)
	{
		setWarningMessage("Can't connect to OSCQuery server", "sync");

		if (getWarningMessage("sync").isEmpty()) NLOGERROR(niceName, "Error with host info request, status code : " << statusCode << ", url : " << url.toString(true));
	}
	else
	{
		clearWarning("sync");
	}
}

void BentoComponentContainer::requestStructure()
{
	URL url("http://" + prop->remoteHost->stringValue());
	StringPairArray responseHeaders;
	int statusCode = 0;

	std::unique_ptr<InputStream> stream(url.createInputStream(
		URL::InputStreamOptions(URL::ParameterHandling::inAddress)
		.withConnectionTimeoutMs(1000)
		.withResponseHeaders(&responseHeaders)
		.withStatusCode(&statusCode)
		.withProgressCallback([this](int, int) { return !isClearing && !Engine::mainEngine->isClearing && !threadShouldExit(); })
	));

	if (threadShouldExit()) return;

#if JUCE_WINDOWS
	if (statusCode != 200)
	{
		NLOGWARNING(niceName, "Failed to request Structure, status code = " + String(statusCode));
		return;
	}
#endif


	if (stream != nullptr)
	{
		String content = stream->readEntireStreamAsString();

		var data = JSON::parse(content);
		if (data.isObject())
		{
			updateTreeFromData(data);
		}
	}
	else
	{
		NLOGWARNING(niceName, "Error with request, status code : " << statusCode << ", url : " << url.toString(true));
	}
}


BentoSubComponent::BentoSubComponent(const String& name, BentoComponentContainer* container) :
	OSCQueryHelpers::OSCQueryValueContainer(name, false, false),
	container(container)
{
	removeControllable(enabled);
	enabled = nullptr;
}

BentoSubComponent::~BentoSubComponent()
{
}

void BentoSubComponent::onControllableAdded(Controllable* c)
{
	EnablingControllableContainer::onControllableAdded(c);

	if (shortName == "strip1") //hardcoded for now
	{
		if (c->shortName == "count")
		{
			container->prop->resolutionRef = ((IntParameter*)c);
			container->prop->resolution->setValue(container->prop->resolutionRef->intValue());
		}
		else if (c->shortName == "brightness")
		{
			container->prop->brightnessRef = ((FloatParameter*)c);
			container->prop->brightness->setRange(container->prop->brightnessRef->minimumValue, container->prop->brightnessRef->maximumValue);
			container->prop->brightness->setValue(container->prop->brightnessRef->floatValue());
		}
		else if (c->shortName == "multiLedMode")
		{
			container->prop->multiLedModeRef = ((EnumParameter*)c);
			container->prop->updateColorsArraySize();
		}
	}
	else if (shortName == "battery")
	{
		if (c->shortName == "batteryLevel")
		{
			container->prop->batteryRef = ((FloatParameter*)c);
			container->prop->battery->setRange(container->prop->batteryRef->minimumValue, container->prop->batteryRef->maximumValue);
			container->prop->battery->setValue(container->prop->batteryRef->floatValue());
		}
		else if (c->shortName == "charging")
		{
			container->prop->chargingRef = ((BoolParameter*)c);
			container->prop->chargingRef->setValue(container->prop->chargingRef->boolValue(), true); //force update
		}
	}
	else if (shortName == "motion")
	{
		if (c->shortName == "enabled")
		{
			container->prop->motionRef = ((BoolParameter*)c);
			container->prop->motionRef->setValue(container->prop->motionRef->boolValue(), true); //force update
		}
	}
	else if (shortName == "settings")
	{
		if (c->shortName == "propID")
		{
			container->prop->idRef = ((IntParameter*)c);
			container->prop->globalID->setValue(container->prop->idRef->intValue());
		}
		else if (c->shortName == "deviceType")
		{
			if (StringParameter* dr = dynamic_cast<StringParameter*>(c))
			{
				if (dr->stringValue() == "Olimex POE Bridge") container->prop->isESPNowBridge = true;
			}
		}
		else if (c->shortName == "saveSettings")
		{
			container->prop->saveSettingsRef = ((Trigger*)c);
		}
	}
	else if (shortName == "streamLayer")
	{
		if (c->shortName == "universe")
		{
			container->prop->universeRef = ((IntParameter*)c);
		}
		else if (c->shortName == "startChannel")
		{
			container->prop->startChannelRef = ((IntParameter*)c);
		}
	}
	else if (shortName == "espnow")
	{
		if (c->shortName == "streamUniverse")
		{
			container->prop->universeRef = ((IntParameter*)c);
		}
		else if (c->shortName == "streamStartChannel")
		{
			container->prop->startChannelRef = ((IntParameter*)c);
		}
	}
	else if (shortName == "systemLayer")
	{
		if (c->shortName == "showBattery")
		{
			container->prop->showBatteryRef = ((BoolParameter*)c);
		}
	}

}
