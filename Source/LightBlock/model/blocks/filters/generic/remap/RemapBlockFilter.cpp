/*
  ==============================================================================

    RemapBlockFilter.cpp
    Created: 18 May 2020 3:27:43pm
    Author:  bkupe

  ==============================================================================
*/

#include "RemapBlockFilter.h"

RemapBlockFilter::RemapBlockFilter(var params) :
    LightBlockFilter(getTypeString(), params)
{
	remapMode = paramsContainer->addEnumParameter("Remap Mode", "Remap Mode");
	remapMode->addOption("Stretch", STRETCH)->addOption("Crop", CROP);

    sourceStart = paramsContainer->addFloatParameter("Source Start", "Start in source", 0, 0, 1);
    sourceEnd = paramsContainer->addFloatParameter("Source End", "End in source", 1, 0, 1);
    targetStart = paramsContainer->addFloatParameter("Target Start", "Start in target", 0, 0, 1);
    targetEnd = paramsContainer->addFloatParameter("Target End", "End in target", .5f, 0, 1);
}

RemapBlockFilter::~RemapBlockFilter()
{
}

void RemapBlockFilter::getColorsInternal(Array<Colour>* result, Prop* p, double time, int id, int resolution, var params)
{
	float bStart = getParamValue<float>(sourceStart, params);
	float bEnd = getParamValue<float>(sourceEnd, params);

	float btStart = getParamValue<float>(targetStart, params);
	float btEnd = getParamValue<float>(targetEnd, params);

	RemapMode m = remapMode->getValueDataAsEnum<RemapMode>();

	int sourceStartLed = bStart * resolution;
	int sourceEndLed = jmax<int>(bEnd * resolution, sourceStartLed + 1);
	int startLed = btStart * resolution;
	int endLed =  btEnd * resolution;

	Array<Colour> originalColours(result->getRawDataPointer(), result->size());
	for (int i = 0; i < resolution; i++)
	{
		Colour col = Colours::black;
		if (i >= startLed && i < endLed)
		{
			int tLed = m == CROP ? i : jmap<float>(i, startLed, endLed, sourceStartLed, sourceEndLed);
			col = originalColours[tLed];
		}

		result->set(i, col); 
	}
}
