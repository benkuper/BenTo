/*
  ==============================================================================

    FlowtoysProp.cpp
    Created: 13 Apr 2018 4:22:33pm
    Author:  Ben

  ==============================================================================
*/

#include "FlowtoysProp.h"

FlowtoysProp::FlowtoysProp(const String & name, var params) :
	Prop(name, params)
{
	sendRate->setValue(50);
	remoteHost = addStringParameter("Remote Host", "IP of the prop on the network", "192.168.0.100");
	button = addBoolParameter("Button", "Is the button on the prop pressed ?", false);
	button->setControllableFeedbackOnly(true);
}

FlowtoysProp::~FlowtoysProp()
{
}

void FlowtoysProp::sendColorsToPropInternal()
{
	const int numLeds = resolution->intValue();
	const int maxLedsPerPacket = 320;

	Array<uint8> data;

	int numPackets = 0;
	for (int i = 0; i < numLeds; i++)
	{	
		data.add(jmin<int>(colors[i].getRed(), 254));
		data.add(jmin<int>(colors[i].getGreen(), 254));
		data.add(jmin<int>(colors[i].getBlue(), 254));

		if (i%maxLedsPerPacket == 0) numPackets++;
	}
	 
	data.add(255);

	int dataSize = numLeds * 3 + 1;
	
	//DBG("Send " << numPackets << " packets");

	for (int i = 0; i < numPackets; i++)
	{
		int offset =  i * maxLedsPerPacket * 3;
		int length = i < numPackets - 1 ? maxLedsPerPacket * 3 : dataSize - offset; //if last packet; put everything
		//DBG("Packet #" << i << ", offset = " << offset << ", length =" << length);
		sender.write(remoteHost->stringValue(), remotePort, data.getRawDataPointer() + offset, length);
		sleep(1);
	}
	
	
}
