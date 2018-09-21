/*
  ==============================================================================

    SmarballProp.cpp
    Created: 21 Sep 2018 8:31:28pm
    Author:  Ben

  ==============================================================================
*/

#include "SmarballProp.h"

SmartballProp::SmartballProp(const String & name, var params) :
	Prop(name, params)
{
	remoteHost = addStringParameter("Remote Host", "IP of the prop on the network", "192.168.0.100");
	accelerometer = addPoint3DParameter("Accelerometer", "Accelerometer data");
	accelerometer->setControllableFeedbackOnly(true);
	resolution->setValue(6);
}

SmartballProp::~SmartballProp()
{
}

void SmartballProp::sendColorsToPropInternal()
{
	const int numLeds = resolution->intValue();
	Array<uint8> data;

	for (int i = 0; i < numLeds; i++)
	{
		data.add(jmin<int>(colors[i].getRed(), 254));
		data.add(jmin<int>(colors[i].getGreen(), 254));
		data.add(jmin<int>(colors[i].getBlue(), 254));
	}

	data.add(255);

	sender.write(remoteHost->stringValue(), remotePort, data.getRawDataPointer(), data.size());
}
