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
	saveAndLoadName = false;
	saveType = false;
}

PatternBlock::~PatternBlock()
{
}

RainbowPattern::RainbowPattern(var params) :
	PatternBlock(getTypeString(), params)
{
	density = paramsContainer->addFloatParameter("Density", "The cycle density of the rainbow", 1, 0.1f, 100);
	offset = paramsContainer->addFloatParameter("Offset", "The offset of the rainbow, in cycles", 0, 0, 20);
	speed = paramsContainer->addFloatParameter("Speed", "The animation speed in cycles/second over the full range of the prop.", 0, 0, 20);
	idOffset = paramsContainer->addFloatParameter("ID Offset", "Offset the hue depending on id of the prop", 0, 0, 10);
	brightness = paramsContainer->addFloatParameter("Brightness", "Brightness of the rainbow", .5f, 0, 1);
}

Array<Colour> RainbowPattern::getColors(int id, int resolution, double time, var params)
{
	Array<Colour> result;
	result.resize(resolution);

	float bDensity = params.getProperty("density", density->floatValue());
	float bOffset = params.getProperty("offset", offset->floatValue());
	float bSpeed = params.getProperty("speed", speed->floatValue());
	float bIdOffset = params.getProperty("idoffset", idOffset->floatValue());
	float bBrightness = params.getProperty("brightness", brightness->floatValue());

	float curOffset = time*bSpeed + bOffset + id*bIdOffset;

	for (int i = 0; i < resolution; i++)
	{
		float rel = fmodf((i * bDensity / resolution) + curOffset, 1);
		result.set(i, Colour::fromHSV(rel, 1, bBrightness, 1));
	}

	return result;
}



NoisePattern::NoisePattern(var params) :
	PatternBlock(getTypeString(), params)
{
	scale = paramsContainer->addFloatParameter("Scale", "", 3, 0.1f, 200);
	speed = paramsContainer->addFloatParameter("Speed", "", 1, 0.1f, 20);
	contrast = paramsContainer->addFloatParameter("Contrast", "", 3, 0, 10);
	brightness = paramsContainer->addFloatParameter("Brightness", "", 0, -1, 1);
	color = paramsContainer->addColorParameter("Color", "", Colours::white);
	idOffset = paramsContainer->addFloatParameter("ID Offset", "Offset the animation depending on id of the prop", 0, 0, 10);
}

Array<Colour> NoisePattern::getColors(int id, int resolution, double time, var params)
{
	Array<Colour> result;
	result.resize(resolution);

	float bScale = params.getProperty("scale", scale->floatValue());
	float bSpeed = params.getProperty("speed",speed->floatValue());
	float bContrast = params.getProperty("contrast", contrast->floatValue());
	float bBrightness = params.getProperty("brightness", brightness->floatValue());
	float bIdOffset = params.getProperty("idoffset", idOffset->floatValue());
	var colorVar = params.getProperty("color", color->value);
	Colour bColor = Colour::fromFloatRGBA(colorVar[0], colorVar[1], colorVar[2], colorVar[3]);

	float curTime = time * bSpeed;

	for (int i = 0; i < resolution; i++)
	{
		float v = (perlin.noise0_1((i*bScale) / resolution, curTime, id*bIdOffset) - .5f)*bContrast + .5f + bBrightness;
		result.set(i, bColor.withMultipliedBrightness(v));
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

Array<Colour> SolidColorPattern::getColors(int id, int resolution, double time, var params)
{
	Array<Colour> result;
	result.resize(resolution);

	var colorVar = params.getProperty("color", color->value);
	Colour bColor = Colour::fromFloatRGBA(colorVar[0], colorVar[1], colorVar[2], colorVar[3]);
	float bHueSpeed = params.getProperty("hueSpeed", hueSpeed->floatValue());
	float bIdOffset = params.getProperty("idoffset", idOffset->floatValue());

	float curTime = time * bHueSpeed + id * bIdOffset;

	Colour c = bColor.withHue(bColor.getHue() + fmodf(curTime, 1));
	result.fill(c);
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

Array<Colour> StrobePattern::getColors(int id, int resolution, double time, var params)
{
	Array<Colour> result;
	result.resize(resolution);

	var colorVar = params.getProperty("color", color->value);
	Colour bColor = Colour::fromFloatRGBA(colorVar[0], colorVar[1], colorVar[2], colorVar[3]);
	float bFrequency = params.getProperty("frequency", frequency->floatValue());
	float bOnOffBalance = params.getProperty("on-OffBalance", onOffBalance->floatValue());
	float bIdOffset = params.getProperty("idoffset", idOffset->floatValue());

	float curTime = time*bFrequency + id * bIdOffset;

	Colour c = fmodf(curTime, 1) < bOnOffBalance ? bColor : Colours::black;
	result.fill(c);
	return result;

}

