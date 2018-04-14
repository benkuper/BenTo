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
