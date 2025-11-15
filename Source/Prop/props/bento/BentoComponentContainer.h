/*
  ==============================================================================

	BentoComponentContainer.h
	Created: 14 Oct 2023 12:42:45pm
	Author:  bkupe

  ==============================================================================
*/

#pragma once

class BentoProp;
class BentoComponentContainer;

class BentoSubComponent :
	public OSCQueryHelpers::OSCQueryValueContainer
{
public:
	BentoSubComponent(const String& name, BentoComponentContainer* container);
	virtual ~BentoSubComponent();

	BentoComponentContainer* container;

	void onControllableAdded(Controllable* c) override;
};

class BentoComponentContainer :
	public ControllableContainer,
	public SimpleWebSocketClientBase::Listener,
	public Thread
{
public:
	BentoComponentContainer(BentoProp* prop);
	virtual ~BentoComponentContainer();


	BentoProp* prop;

	Trigger* listenNoneTrigger;

	std::unique_ptr<SimpleWebSocketClientBase> wsClient;
	int wsPort;

	bool isUpdatingStructure;
	Array<Controllable*> noFeedbackList;

	void setupWSClient();
	void closeWSClient();
	//Script

	void onControllableFeedbackUpdate(ControllableContainer* cc, Controllable* c) override;
	void onContainerParameterChanged(Parameter* p) override;
	void onContainerTriggerTriggered(Trigger* t) override;

	virtual void syncData(bool force = false);
	virtual void updateTreeFromData(var data);

	void connectionOpened() override;
	void connectionClosed(int status, const String& reason) override;
	void connectionError(const String& errorMessage) override;

	void dataReceived(const MemoryBlock& data) override;
	void messageReceived(const String& message) override;


	// Inherited via Thread
	virtual void run() override;
	virtual void requestHostInfo();
	virtual void requestStructure();

};
