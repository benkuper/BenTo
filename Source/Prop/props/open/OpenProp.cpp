/*
  ==============================================================================

    OpenProp.cpp
    Created: 7 Mar 2020 4:58:39pm
    Author:  bkupe

  ==============================================================================
*/

#include "OpenProp.h"

OpenProp::OpenProp(StringRef name, var params) :
	Prop(name, "Open", params),
	script(this)
{
	addChildControllableContainer(&script);
}

OpenProp::~OpenProp()
{
}

void OpenProp::sendColorsToPropInternal()
{
	Array<var> params;
	var colorData;
	for (auto& c : colors)
	{
		var cData;
		cData.append(c.getFloatRed());
		cData.append(c.getFloatGreen());
		cData.append(c.getFloatBlue());
		cData.append(c.getFloatAlpha());
		colorData.append(cData);
	}
	params.add(colorData, resolution->intValue());

	script.callFunction("sendColors", params);
}

var OpenProp::getJSONData()
{
	var data = Prop::getJSONData();
	data.getDynamicObject()->setProperty("script", script.getJSONData());
	return data;
}

void OpenProp::loadJSONDataItemInternal(var data)
{
	Prop::loadJSONDataItemInternal(data);
	script.loadJSONData(data.getProperty("script", var()));
}
