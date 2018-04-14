/*
  ==============================================================================

    FlowtoysProp.h
    Created: 13 Apr 2018 4:22:33pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "../../Prop.h"

class FlowtoysProp :
	public Prop
{
public:
	FlowtoysProp(const String &name = "Flowtoys Prop", var params = var());
	~FlowtoysProp();

	StringParameter * remoteHost;
	BoolParameter * button;
	DatagramSocket sender;

	const int remotePort = 8888; //Bento udp streaming is done on this port, period.

	virtual void sendColorsToPropInternal() override;

};