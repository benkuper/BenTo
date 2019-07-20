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

PatternBlock::PatternBlock(const String& name, var params) :
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
	density = paramsContainer->addFloatParameter("Density", "The cycle density of the rainbow", 1, 0.1f, 10);
	offset = paramsContainer->addFloatParameter("Offset", "The offset of the rainbow, in cycles", 0, 0, 20);
	speed = paramsContainer->addFloatParameter("Speed", "The animation speed in cycles/second over the full range of the prop.", -10, 0, 10);
	idOffset = paramsContainer->addFloatParameter("ID Offset", "Offset the hue depending on id of the prop", 0, 0, 10);
	brightness = paramsContainer->addFloatParameter("Brightness", "Brightness of the rainbow", .75f, 0, 1);
}

void RainbowPattern::getColorsInternal(Array<Colour>* result, Prop* p, double time, int id, int resolution, var params)
{
	float bDensity = getParamValue<float>(density, params);
	float bOffset = getParamValue<float>(offset, params);
	float bSpeed = getParamValue<float>(speed, params);
	float bIdOffset = getParamValue<float>(idOffset, params);
	float bBrightness = getParamValue<float>(brightness, params);

	float curOffset = time * bSpeed + bOffset + id * bIdOffset;

	for (int i = 0; i < resolution; i++)
	{
		float rel = fmodf((i * bDensity / resolution) + curOffset, 1);
		result->set(i, Colour::fromHSV(rel, 1, bBrightness, 1));
	}
}



NoisePattern::NoisePattern(var params) :
	PatternBlock(getTypeString(), params)
{
	scale = paramsContainer->addFloatParameter("Scale", "", 3, 0.1f, 200);
	speed = paramsContainer->addFloatParameter("Speed", "", 1, 0.1f, 20);
	contrast = paramsContainer->addFloatParameter("Contrast", "", 3, 0, 10);
	brightness = paramsContainer->addFloatParameter("Brightness", "", 0, -1, 1);
	color = paramsContainer->addColorParameter("Color", "", Colours::white);
	bgColor = paramsContainer->addColorParameter("Background Color", "", Colours::black);
	idOffset = paramsContainer->addFloatParameter("ID Offset", "Offset the animation depending on id of the prop", 0, 0, 10);
}

void NoisePattern::getColorsInternal(Array<Colour>* result, Prop* p, double time, int id, int resolution, var params)
{

	float bScale = getParamValue<float>(scale, params);
	float bSpeed = getParamValue<float>(speed, params);
	float bContrast = getParamValue<float>(contrast, params);
	float bBrightness = getParamValue<float>(brightness, params);
	float bIdOffset = getParamValue<float>(idOffset, params);

	var colorVar = getParamValue<var>(color, params);
	Colour bColor = Colour::fromFloatRGBA(colorVar[0], colorVar[1], colorVar[2], colorVar[3]);
	var bgColorVar = getParamValue<var>(bgColor, params);
	Colour bbgColor = Colour::fromFloatRGBA(bgColorVar[0], bgColorVar[1], bgColorVar[2], bgColorVar[3]);
	float curTime = time * bSpeed;

	for (int i = 0; i < resolution; i++)
	{
		float v = (perlin.noise0_1((i * bScale) / resolution, curTime, id * bIdOffset) - .5f) * bContrast + .5f + bBrightness;
		result->set(i, bbgColor.interpolatedWith(bColor, v));
	}
}


SolidColorPattern::SolidColorPattern(var params) :
	PatternBlock(getTypeString(), params)
{
	color = paramsContainer->addColorParameter("Color", "", Colours::white);
	brightness = paramsContainer->addFloatParameter("Brightness", "", 0.5, 0, 1);
	hueSpeed = paramsContainer->addFloatParameter("Hue Speed", "The animation speed of the hue, in full spectrum cycle / second", 0, 0, 20);
	idOffset = paramsContainer->addFloatParameter("ID Offset", "Offset the hue depending on id of the prop", 0, 0, 10);

}

void SolidColorPattern::getColorsInternal(Array<Colour>* result, Prop* p, double time, int id, int resolution, var params)
{
	var colorVar = getParamValue<var>(color, params);
	Colour bColor = Colour::fromFloatRGBA(colorVar[0], colorVar[1], colorVar[2], colorVar[3]);
	float bBrightness = getParamValue<float>(brightness, params);
	float bHueSpeed = getParamValue<float>(hueSpeed, params);
	float bIdOffset = getParamValue<float>(idOffset, params);

	float curTime = time * bHueSpeed + id * bIdOffset;

	Colour c = bColor.withHue(bColor.getHue() + fmodf(curTime, 1));
	result->fill(c.withMultipliedBrightness(bBrightness));
}

StrobePattern::StrobePattern(var params) :
	PatternBlock(getTypeString(), params)
{
	color = paramsContainer->addColorParameter("Color", "", Colours::white);
	color2 = paramsContainer->addColorParameter("Color 2", "", Colours::black);
	frequency = paramsContainer->addFloatParameter("Frequency", "", 1, .01f, 100);
	onOffBalance = paramsContainer->addFloatParameter("On-Off Balance", "The balance between on and off time. 0.5s means equals on and off time. .8 means 80% on time, 20% off time.", .5f, 0, 1);
	idOffset = paramsContainer->addFloatParameter("ID Offset", "Offset the timing depending on id of the prop", 0, 0, 10);
}

void StrobePattern::getColorsInternal(Array<Colour>* result, Prop* p, double time, int id, int resolution, var params)
{
	var colorVar = getParamValue<var>(color, params);
	Colour bColor = Colour::fromFloatRGBA(colorVar[0], colorVar[1], colorVar[2], colorVar[3]);
	var color2Var = getParamValue<var>(color2, params);
	Colour bColor2 = Colour::fromFloatRGBA(color2Var[0], color2Var[1], color2Var[2], color2Var[3]);

	float bFrequency = getParamValue<float>(frequency, params);
	float bOnOffBalance = getParamValue<float>(onOffBalance, params);
	float bIdOffset = getParamValue<float>(idOffset, params);

	float curTime = time * bFrequency + id * bIdOffset;

	Colour c = fmodf(curTime, 1) < bOnOffBalance ? bColor : bColor2;
	result->fill(c);
}

PointPattern::PointPattern(var params) :
	PatternBlock(getTypeString(), params)
{
	position = paramsContainer->addFloatParameter("Position", "Position of the point", .5f, 0, 1);
	size = paramsContainer->addFloatParameter("Size", "Size of the point", .25f, 0, 2);
	fade = paramsContainer->addFloatParameter("Fade", "The fading of the point", 1, 0, 1);
	color = paramsContainer->addColorParameter("Color", "The color of the point", Colours::white);
	bgColor = paramsContainer->addColorParameter("Background Color", "The color of the background", Colours::black);
	extendNum = paramsContainer->addIntParameter("Num Props", "The number of props the point is navigating through", 1, 1);
	invertEvens = paramsContainer->addBoolParameter("Invert Evens", "If checked, swap the direction of props with even IDs", false);
	invertOdds = paramsContainer->addBoolParameter("Invert Odds", "If checked, swap the direction of props with odd IDs", false);
}

void PointPattern::getColorsInternal(Array<Colour>* result, Prop* p, double time, int id, int resolution, var params)
{
	var colorVar = getParamValue<var>(color, params);
	Colour bColor = Colour::fromFloatRGBA(colorVar[0], colorVar[1], colorVar[2], colorVar[3]);

	var bgColorVar = getParamValue<var>(bgColor, params);
	Colour bBGColor = Colour::fromFloatRGBA(bgColorVar[0], bgColorVar[1], bgColorVar[2], bgColorVar[3]);
	
	float bPosition = getParamValue<float>(position, params);
	float bSize = getParamValue<float>(size, params);
	float bFade = getParamValue<float>(fade, params);
	int bExtend = getParamValue<int>(extendNum, params);
	bool bInvertEvens = getParamValue<bool>(invertEvens, params);
	bool bInvertOdds = getParamValue<bool>(invertOdds, params);

	float extendPos = bPosition * bExtend;

	float relPos = (extendPos - id%bExtend) * resolution;
	float relStart = jmax<int>(relPos - (bSize * resolution / 2.f), 0);
	float relEnd = jmin<int>(relPos + (bSize * resolution / 2.f), resolution);
	float relSize = bSize * resolution * bExtend;

	result->fill(bBGColor);

	for (int i = relStart; i <= relEnd && i < resolution; i++)
	{
		float diff = 1 - (fabsf(i - relPos) * 1.f / (relSize /(bFade*2)));
		bool invert = id % 2 == 0 ? bInvertEvens : bInvertOdds;
		result->set(invert?resolution-i:i, bBGColor.interpolatedWith(bColor, diff));
	}
}

MultiPointPattern::MultiPointPattern(var params) :
	PatternBlock(getTypeString(), params)
{
	numPoints = paramsContainer->addIntParameter("Num Points", "The number of lines per prop", 3, 1);
	position = paramsContainer->addFloatParameter("Position", "Position of the point", .25f, 0, 1);
	size = paramsContainer->addFloatParameter("Size", "Size of the point", .5f, 0, 1);
	fade = paramsContainer->addFloatParameter("Fade", "The fading of the point", 1, 0, 1);
	color = paramsContainer->addColorParameter("Color", "The color of the point", Colours::white);
	bgColor = paramsContainer->addColorParameter("Background Color", "The color of the background", Colours::black);
}

void MultiPointPattern::getColorsInternal(Array<Colour>* result, Prop* p, double time, int id, int resolution, var params)
{
	var colorVar = getParamValue<var>(color, params);
	Colour bColor = Colour::fromFloatRGBA(colorVar[0], colorVar[1], colorVar[2], colorVar[3]);
	
	var bgColorVar = getParamValue<var>(bgColor, params);
	Colour bBGColor = Colour::fromFloatRGBA(bgColorVar[0], bgColorVar[1], bgColorVar[2], bgColorVar[3]);

	float bPosition = getParamValue<float>(position, params);
	float bSize = getParamValue<float>(size, params);
	float bFade = getParamValue<float>(fade, params);
	int bNumPoints = getParamValue<int>(numPoints, params); 
	
	for (int i = 0; i < resolution; i++)
	{
		float relI = fmodf(i * bNumPoints * 1.0f / resolution, 1);
		float fac = 0;
		for (int j = -1; j <= 1; j++)
		{
			float relDiff = fabsf(relI + j - bPosition);
			if (relDiff <= bSize/2)
			{
				fac += jlimit(0.f, 1.f, 1 - (relDiff * bFade) / (bSize / 2));
			}
		}

		result->set(i, bBGColor.interpolatedWith(bColor, fac));
	}
}
