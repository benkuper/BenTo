/*
  ==============================================================================

    OpenSerialProp.cpp
    Created: 7 Mar 2020 4:56:09pm
    Author:  bkupe

  ==============================================================================
*/

#include "OpenSerialProp.h"

OpenSerialProp::OpenSerialProp(var params) :
	OpenProp(getTypeString(), params)
{
	deviceParam = new SerialDeviceParameter("Port", "Serial device port", true);
	deviceParam->openBaudRate = 115200;
	ioCC.addParameter(deviceParam);
	scriptObject.setMethod("send", &OpenSerialProp::sendBytesFromScript);
}

OpenSerialProp::~OpenSerialProp()
{
}

var OpenSerialProp::sendBytesFromScript(const var::NativeFunctionArgs& a)
{
	OpenSerialProp* p = getObjectFromJS<OpenSerialProp>(a);
	if (!p->enabled->boolValue()) return var();
	if (a.numArguments == 0) return var();
	SerialDevice* d = p->deviceParam->getDevice();

	Array<uint8> data;
	for (int i = 0; i < a.numArguments; i++)
	{
		data.add((uint8)(int)a.arguments[i]);
	}
	if (d->isOpen())
	{
		d->writeBytes(data);
	}
	
	return var();
}
