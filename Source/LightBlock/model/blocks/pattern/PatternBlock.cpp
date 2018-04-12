/*
  ==============================================================================

	PatternBlock.cpp
	Created: 10 Apr 2018 6:58:57pm
	Author:  Ben

  ==============================================================================
*/

#include "PatternBlock.h"
#include "LightBlock/LightBlock.h"
#include "Prop/Prop.h"

PatternBlock::PatternBlock(const String &name, var params) :
	LightBlockModel(name == "" ? getTypeString() : name, params)
{
}

PatternBlock::~PatternBlock()
{
}

RainbowPattern::RainbowPattern(var params) : 
	PatternBlock(getTypeString(), params)
{
	density = paramsContainer->addFloatParameter("Density", "The cycle density of the rainbow", 1, 0.1f, 10);
	offset = paramsContainer->addFloatParameter("Offset", "The offset of the rainbow, in cycles", 0, 0, 20);
	speed = paramsContainer->addFloatParameter("Speed", "The animation speed in cycles/second over the full range of the prop.", 0, 0, 20);
	idOffset = paramsContainer->addFloatParameter("ID Offset", "Offset the hue depending on id of the prop", 0, 0, 10);
}

Array<Colour> RainbowPattern::getColors(LightBlock * block, var params)
{
	float bDensity = block->paramsContainer.getParameterByName("density")->floatValue();
	float bOffset = block->paramsContainer.getParameterByName("offset")->floatValue();
	float bSpeed = block->paramsContainer.getParameterByName("speed")->floatValue();
	float bIdOffset = block->paramsContainer.getParameterByName("idoffset")->floatValue();

	int numLeds = block->prop->resolution->intValue();
	Array<Colour> result;

	float curOffset = (Time::getApproximateMillisecondCounter() / 1000.0f)*bSpeed + bOffset + block->prop->id->intValue()*bIdOffset;

	for (int i = 0; i < numLeds; i++)
	{
		float rel = fmodf((i * bDensity / numLeds) + curOffset, 1);
		result.add(Colour::fromHSV(rel, 1, 1, 1));
	}

	return result;
}



NoisePattern::NoisePattern(var params) :
	PatternBlock(getTypeString(), params)
{
	scale = paramsContainer->addFloatParameter("Scale", "", 1, 0.1f, 20);
	speed = paramsContainer->addFloatParameter("Speed", "", 1, 0.1f, 20);
	contrast = paramsContainer->addFloatParameter("Contrast", "", 1,0,5);
	brightness = paramsContainer->addFloatParameter("Brightness", "",0,-1,1);
	color = paramsContainer->addColorParameter("Color", "", Colours::white);
	idOffset = paramsContainer->addFloatParameter("ID Offset", "Offset the animation depending on id of the prop", 0, 0, 10);
}

Array<Colour> NoisePattern::getColors(LightBlock * block, var params)
{
	int numLeds = block->prop->resolution->intValue();
	Array<Colour> result;

	float bScale = block->paramsContainer.getParameterByName("scale")->floatValue();
	float bSpeed = block->paramsContainer.getParameterByName("speed")->floatValue();
	float bContrast = block->paramsContainer.getParameterByName("contrast")->floatValue();
	float bBrightness = block->paramsContainer.getParameterByName("brightness")->floatValue();
	float bIdOffset = block->paramsContainer.getParameterByName("idoffset")->floatValue();
	Colour bColor = dynamic_cast<ColorParameter *>(block->paramsContainer.getControllableByName("color"))->getColor();
	
	float curTime = (Time::getApproximateMillisecondCounter() / 1000.0f)*bSpeed;

	for (int i = 0; i < numLeds; i++)
	{
		float v = (perlin.noise0_1(((i*bScale) + (block->prop->id->intValue()*bIdOffset)) / numLeds, curTime) - .5f)*bContrast + .5f + bBrightness;
		result.add(bColor.withBrightness(v));
	}

	return result;
}


SolidColorPattern::SolidColorPattern(var params) : 
	PatternBlock(getTypeString(), params)
{
	color = paramsContainer->addColorParameter("Color", "", Colours::white);
	hueSpeed = paramsContainer->addFloatParameter("Hue Speed", "The animation speed of the hue, in full spectrum cycle / second", 0, 0, 20);
	idOffset = paramsContainer->addFloatParameter("ID Offset", "Offset the hue depending on id of the prop", 0, 0, 10);
	
}

Array<Colour> SolidColorPattern::getColors(LightBlock * block, var params)
{
	int numLeds = block->prop->resolution->intValue();
	Array<Colour> result;

	Colour bColor = dynamic_cast<ColorParameter *>(block->paramsContainer.getControllableByName("color"))->getColor();
	float bHueSpeed = block->paramsContainer.getParameterByName("hueSpeed")->floatValue();
	float bIdOffset = block->paramsContainer.getParameterByName("idoffset")->floatValue();

	float curTime = (Time::getApproximateMillisecondCounter() / 1000.0f)*bHueSpeed + (block->prop->id->intValue()*bIdOffset);
	
	Colour c = bColor.withHue(bColor.getHue() + fmodf(curTime, 1));

	for (int i = 0; i < numLeds; i++)
	{
		result.add(c);
	}

	return result;
}

StrobePattern::StrobePattern(var params) : 
	PatternBlock(getTypeString(), params)
{
	color = paramsContainer->addColorParameter("Color", "", Colours::white);
	frequency = paramsContainer->addFloatParameter("Frequency", "", 1, .01f, 100);
	onOffBalance = paramsContainer->addFloatParameter("On-Off Balance", "The balance between on and off time. 0.5s means equals on and off time. .8 means 80% on time, 20% off time.", .5f, 0, 1);
	idOffset = paramsContainer->addFloatParameter("ID Offset", "Offset the timing depending on id of the prop", 0, 0, 10);
}

Array<Colour> StrobePattern::getColors(LightBlock * block, var params)
{
	int numLeds = block->prop->resolution->intValue();
	Array<Colour> result;

	Colour bColor = dynamic_cast<ColorParameter *>(block->paramsContainer.getControllableByName("color"))->getColor();
	float bFrequency = block->paramsContainer.getParameterByName("frequency")->floatValue();
	float bOnOffBalance = block->paramsContainer.getParameterByName("on-OffBalance")->floatValue();
	float bIdOffset = block->paramsContainer.getParameterByName("idoffset")->floatValue();

	float curTime = (Time::getMillisecondCounterHiRes() / 1000.0f)*bFrequency+ (block->prop->id->intValue()*bIdOffset);

	Colour c = fmodf(curTime, 1) < bOnOffBalance ? bColor : Colours::black;

	for (int i = 0; i < numLeds; i++)
	{
		result.add(c);
	}

	return result;
}

