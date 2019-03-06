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
	virtual void sendColorsToPropInternal() override;

	virtual void uploadBakedData(BakeData data) override;
	virtual void exportBakedData(BakeData data) override;

	static bool uploadProgressCallback(void * prop, int bytesSent, int totalBytes);
};