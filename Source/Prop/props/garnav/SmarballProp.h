/*
  ==============================================================================

    SmarballProp.h
    Created: 21 Sep 2018 8:31:28pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "../../Prop.h"

class SmartballProp :
	public Prop
{
public:
	SmartballProp(const String &name = "SmartBall", var params = var());
	~SmartballProp();

	StringParameter * remoteHost;
	Point3DParameter * accelerometer;
	DatagramSocket sender;

	const int remotePort = 8888; //Bento udp streaming is done on this port


	String getTypeString() const override { return "SmartBall"; }
	static SmartballProp * create(var params) { return new SmartballProp(params); }

	virtual void sendColorsToPropInternal() override;

};