/*
  ==============================================================================

    BentoProp.h
    Created: 5 Mar 2019 11:01:56pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once

#include "../../Prop.h"

class BentoProp :
	public Prop
{
public:
	BentoProp(StringRef name = "Bento", StringRef family = "Family", var params = var());
	virtual ~BentoProp();
	StringParameter * remoteHost;

	const int remotePort = 8888;
	DatagramSocket sender;
	OSCSender oscSender;

	void onContainerParameterChangedInternal(Parameter * p) override;
	void onControllableFeedbackUpdateInternal(ControllableContainer * cc, Controllable *c) override;

	virtual void sendColorsToPropInternal() override;

	virtual void uploadBakedData(BakeData data) override;
	virtual void exportBakedData(BakeData data) override;

	virtual void loadBake(StringRef fileName, bool autoPlay = false) override;
	virtual void playBake(float time = -1) override;
	virtual void pauseBakePlaying() override;
	virtual void resumeBakePlaying() override {}
	virtual void seekBakePlaying(float time) override;
	virtual void stopBakePlaying() override;

	static bool uploadProgressCallback(void* prop, int bytesSent, int totalBytes);
	static bool uploadMetaDataProgressCallback(void * prop, int bytesSent, int totalBytes);

	virtual void sendPing() override;
	virtual void powerOffProp() override;
	virtual void resetProp() override;

};