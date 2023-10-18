/*
  ==============================================================================

	BentoComponentContainer.cpp
	Created: 14 Oct 2023 12:42:45pm
	Author:  bkupe

  ==============================================================================
*/

#include "Prop/PropIncludes.h"
#include "BentoComponentContainer.h"

BentoComponentContainer::BentoComponentContainer(BentoProp* prop) :
	ControllableContainer("Components"),
	Thread("Bento OSCQuery"),
	prop(prop),
	wsPort(80),
	isUpdatingStructure(false)
{
	startThread();
	startTimer(5000); //would be better in PropManager
}

BentoComponentContainer::~BentoComponentContainer()
{
	if (wsClient != nullptr) wsClient->stop();
	stopThread(2000);
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
	String url = host + ":81/";
	DBG("Setting up client at " << url);
	wsClient->start(url);
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


void BentoComponentContainer::syncData()
{
	if (isCurrentlyLoadingData || Engine::mainEngine->isLoadingFile) return;

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


void BentoComponentContainer::timerCallback()
{
	if (!prop->isConnected->boolValue())
	{
		stopThread(300);
		startThread();
	}
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
		.withConnectionTimeoutMs(200)
		.withResponseHeaders(&responseHeaders)
		.withStatusCode(&statusCode)
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
			wsPort = data.getProperty("WS_PORT", wsPort);
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
	URL url("http://" + prop->remoteHost->stringValue() + "?config=0");
	StringPairArray responseHeaders;
	int statusCode = 0;

	std::unique_ptr<InputStream> stream(url.createInputStream(
		URL::InputStreamOptions(URL::ParameterHandling::inAddress)
		.withConnectionTimeoutMs(200)
		.withResponseHeaders(&responseHeaders)
		.withStatusCode(&statusCode)
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
			container->prop->brightness->setValue(container->prop->brightnessRef->floatValue());
		}
	}

}
