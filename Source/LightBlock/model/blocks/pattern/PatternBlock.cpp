/*
  ==============================================================================

	PatternBlock.cpp
	Created: 10 Apr 2018 6:58:57pm
	Author:  Ben

  ==============================================================================
*/

#include "LightBlock/LightBlockIncludes.h"
#include "PatternBlock.h"

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
	brightness = paramsContainer->addFloatParameter("Brightness", "Brightness of the rainbow", .75f, 0, 1);
	offset = paramsContainer->addFloatParameter("Offset", "The offset of the rainbow, in cycles", 0);
	speed = paramsContainer->addFloatParameter("Speed", "The animation speed in cycles/second over the full range of the prop.", 0);
	density = paramsContainer->addFloatParameter("Density", "The cycle density of the rainbow", 1);
	idOffset = paramsContainer->addFloatParameter("ID Offset", "Offset the hue depending on id of the prop", 0);
}

void RainbowPattern::getColorsInternal(Array<Colour>* result, Prop* p, double time, int id, int resolution, var params)
{
	double bDensity = getParamValue<double>(density, params);
	double bOffset = getParamValue<double>(offset, params);
	double bSpeed = getParamValue<double>(speed, params);
	double bIdOffset = getParamValue<double>(idOffset, params);
	double bBrightness = getParamValue<double>(brightness, params);

	double curOffset = time * bSpeed + bOffset + id * bIdOffset;

	for (int i = 0; i < resolution; i++)
	{
		double rel = fmod((1 - (i * 1.0 / resolution)) * bDensity + curOffset, 1);
		result->set(i, Colour::fromHSV(rel, 1.f, bBrightness, 1.f));
	}
}



NoisePattern::NoisePattern(var params) :
	PatternBlock(getTypeString(), params)
{
	brightness = paramsContainer->addFloatParameter("Brightness", "", 1, 0, 1);
	color = paramsContainer->addColorParameter("Color", "", Colours::white);
	bgColor = paramsContainer->addColorParameter("Background Color", "", Colours::black);
	balance = paramsContainer->addFloatParameter("Balance", "The balance between colors", 0, -1, 1);
	contrast = paramsContainer->addFloatParameter("Contrast", "", 3);
	scale = paramsContainer->addFloatParameter("Scale", "", 3);
	speed = paramsContainer->addFloatParameter("Speed", "", 1);
	idOffset = paramsContainer->addFloatParameter("ID Offset", "Offset the animation depending on id of the prop", 0);
}

void NoisePattern::getColorsInternal(Array<Colour>* result, Prop* p, double time, int id, int resolution, var params)
{

	double bScale = getParamValue<double>(scale, params);
	double bSpeed = getParamValue<double>(speed, params);
	double bContrast = getParamValue<double>(contrast, params);
	double bBrightness = getParamValue<double>(brightness, params);
	double bBalance = getParamValue<double>(balance, params);
	double bIdOffset = getParamValue<double>(idOffset, params);

	var colorVar = getParamValue<var>(color, params);
	if (colorVar.size() < 4) return;

	Colour bColor = Colour::fromFloatRGBA(colorVar[0], colorVar[1], colorVar[2], colorVar[3]);
	var bgColorVar = getParamValue<var>(bgColor, params);
	Colour bbgColor = Colour::fromFloatRGBA(bgColorVar[0], bgColorVar[1], bgColorVar[2], bgColorVar[3]);
	double curTime = time * bSpeed;

	for (int i = 0; i < resolution; i++)
	{
		double v = (perlin.noise0_1((i * bScale) / resolution + id * bIdOffset, curTime) - .5f) * bContrast + .5f + bBalance * 2;
		result->set(i, bbgColor.interpolatedWith(bColor, v).withMultipliedBrightness(bBrightness));
	}
}


SolidColorPattern::SolidColorPattern(var params) :
	PatternBlock(getTypeString(), params)
{
	brightness = paramsContainer->addFloatParameter("Brightness", "", 1, 0, 1);
	color = paramsContainer->addColorParameter("Color", "", Colours::red);
	hueSpeed = paramsContainer->addFloatParameter("Hue Speed", "The animation speed of the hue, in full spectrum cycle / second", 0);
	idOffset = paramsContainer->addFloatParameter("ID Offset", "Offset the hue depending on id of the prop", 0);

}

void SolidColorPattern::getColorsInternal(Array<Colour>* result, Prop* p, double time, int id, int resolution, var params)
{
	var colorVar = getParamValue<var>(color, params);
	Colour bColor = Colour::fromFloatRGBA(colorVar[0], colorVar[1], colorVar[2], colorVar[3]);
	double bBrightness = getParamValue<double>(brightness, params);
	double bHueSpeed = getParamValue<double>(hueSpeed, params);
	double bIdOffset = getParamValue<double>(idOffset, params);

	double curTime = time * bHueSpeed + id * bIdOffset;

	Colour c = bColor.withHue(bColor.getHue() + fmod(curTime, 1));
	result->fill(c.withMultipliedBrightness(bBrightness));
}

StrobePattern::StrobePattern(var params) :
	PatternBlock(getTypeString(), params)
{
	brightness = paramsContainer->addFloatParameter("Brightness", "", 1, 0, 1);
	color = paramsContainer->addColorParameter("Color", "", Colours::white);
	color2 = paramsContainer->addColorParameter("Color 2", "", Colours::black);
	frequency = paramsContainer->addFloatParameter("Frequency", "", 1);
	onOffBalance = paramsContainer->addFloatParameter("On-Off Balance", "The balance between on and off time. 0.5s means equals on and off time. .8 means 80% on time, 20% off time.", .5f, 0, 1);
	fadeIn = paramsContainer->addFloatParameter("Fade In", "Fade in", 0, 0, 1);
	fadeOut = paramsContainer->addFloatParameter("Fade Out", "Fade out", 0, 0, 1);
	offset = paramsContainer->addFloatParameter("Offset", "Offset the timing", 0);
	idOffset = paramsContainer->addFloatParameter("ID Offset", "Offset the timing depending on id of the prop", 0);

}

void StrobePattern::getColorsInternal(Array<Colour>* result, Prop* p, double time, int id, int resolution, var params)
{
	var colorVar = getParamValue<var>(color, params);
	Colour bColor = Colour::fromFloatRGBA(colorVar[0], colorVar[1], colorVar[2], colorVar[3]);
	var color2Var = getParamValue<var>(color2, params);
	Colour bColor2 = Colour::fromFloatRGBA(color2Var[0], color2Var[1], color2Var[2], color2Var[3]);

	double bBrightness = getParamValue<double>(brightness, params);
	double bFrequency = getParamValue<double>(frequency, params);
	double bOnOffBalance = getParamValue<double>(onOffBalance, params);
	double fin = getParamValue<double>(fadeIn, params);
	double fout = getParamValue<double>(fadeOut, params);
	double bOffset = getParamValue<double>(offset, params);
	double bIdOffset = getParamValue<double>(idOffset, params);

	double finTime = fin * (1 - bOnOffBalance);
	double foutTime = fout * (1 - bOnOffBalance);

	double curTime = time * bFrequency - id * bIdOffset - bOffset;
	double relTime = fmod(curTime, 1);
	if (relTime < 0) relTime += 1;

	double offRelIn = finTime == 0 ? 0 : jlimit<double>(0, 1, jmap<double>(relTime, 1 - finTime, 1, 0, 1));
	double offRelOut = foutTime == 0 ? 0 : jlimit<double>(0, 1, jmap<double>(relTime, bOnOffBalance, bOnOffBalance + foutTime, 1, 0));

	double weight = jmax(offRelIn, offRelOut);
	Colour c = relTime < bOnOffBalance ? bColor : bColor2.interpolatedWith(bColor, weight);

	DBG("Time " << time << " > weight " << bOnOffBalance);

	result->fill(c.withMultipliedBrightness(bBrightness));
}

PointPattern::PointPattern(var params) :
	PatternBlock(getTypeString(), params)
{
	brightness = paramsContainer->addFloatParameter("Brightness", "", 1, 0, 1);
	color = paramsContainer->addColorParameter("Color", "The color of the point", Colours::white);
	bgColor = paramsContainer->addColorParameter("Background Color", "The color of the background", Colours::black);
	position = paramsContainer->addFloatParameter("Position", "Position of the point", .5f);
	size = paramsContainer->addFloatParameter("Size", "Size of the point", .25f, 0);
	fade = paramsContainer->addFloatParameter("Fade", "The fading of the point", 1, 0, 1);
	loop = paramsContainer->addBoolParameter("Loop", "If checked, content that is overflowing will be showed on the other side of the prop. Useful for cycling objects like hoops.", true);
	extendNum = paramsContainer->addIntParameter("Num Props", "The number of props the point is navigating through", 1, 1);
	invertEvens = paramsContainer->addBoolParameter("Invert Evens", "If checked, swap the direction of props with even IDs", false);
	invertOdds = paramsContainer->addBoolParameter("Invert Odds", "If checked, swap the direction of props with odd IDs", false);
}

void PointPattern::getColorsInternal(Array<Colour>* result, Prop* p, double time, int id, int resolution, var params)
{
	var bgColorVar = getParamValue<var>(bgColor, params);
	Colour bBGColor = Colour::fromFloatRGBA(bgColorVar[0], bgColorVar[1], bgColorVar[2], bgColorVar[3]);

	double bBrightness = getParamValue<double>(brightness, params);
	result->fill(bBGColor.withMultipliedBrightness(bBrightness));


	double bPosition = getParamValue<double>(position, params);
	double bSize = getParamValue<double>(size, params);
	int bExtend = getParamValue<int>(extendNum, params);

	int bLoop = getParamValue<bool>(loop, params);

	if (bLoop)
	{
		bPosition = fmod(bPosition, bExtend);
		params.getDynamicObject()->setProperty("position", bPosition);
	}

	fillPoint(result, p, time, id, resolution, params);

	if (bLoop)
	{
		if (bPosition - bSize / 2 < 0)
		{
			params.getDynamicObject()->setProperty("position", fmod(bPosition, bExtend) + bExtend);
			fillPoint(result, p, time, id, resolution, params);
		}
		else if (bPosition + bSize / 2 > bExtend)
		{
			params.getDynamicObject()->setProperty("position", fmod(bPosition, bExtend) - bExtend);
			fillPoint(result, p, time, id, resolution, params);
		}
	}
}

void PointPattern::fillPoint(Array<Colour>* result, Prop* p, double time, int id, int resolution, var params)
{
	double bPosition = getParamValue<double>(position, params);
	double bSize = getParamValue<double>(size, params);
	double bFade = getParamValue<double>(fade, params);
	int bExtend = getParamValue<int>(extendNum, params);
	bool bInvertEvens = getParamValue<bool>(invertEvens, params);
	bool bInvertOdds = getParamValue<bool>(invertOdds, params);

	double relSize = bSize * resolution;

	bool invert = id % 2 == 0 ? bInvertEvens : bInvertOdds;

	double relPos = (bPosition - id % bExtend) * resolution;
	double relStart = jmax<int>(relPos - (bSize * resolution / 2.f), 0);
	double relEnd = jmin<int>(relPos + (bSize * resolution / 2.f), resolution);

	var colorVar = getParamValue<var>(color, params);
	Colour bColor = Colour::fromFloatRGBA(colorVar[0], colorVar[1], colorVar[2], colorVar[3]);

	double bBrightness = getParamValue<double>(brightness, params);


	for (int i = relStart; i <= relEnd && i < resolution; i++)
	{
		float diff = 1 - (fabsf(i - relPos) * 1.f / (relSize / (bFade * 2)));

		int index = invert ? resolution - 1 - i : i;
		Colour c = result->getUnchecked(index).interpolatedWith(bColor, diff);
		result->set(index, c.withMultipliedBrightness(bBrightness));
	}
}

RangePattern::RangePattern(var params) :
	PatternBlock(getTypeString(), params)
{
	brightness = paramsContainer->addFloatParameter("Brightness", "", 1, 0, 1);
	color = paramsContainer->addColorParameter("Color", "The color of the point", Colours::white);
	bgColor = paramsContainer->addColorParameter("Background Color", "The color of the background", Colours::black);
	start = paramsContainer->addFloatParameter("Start", "Range start", 0);
	end = paramsContainer->addFloatParameter("End", "Range end", .5f);
	fade = paramsContainer->addFloatParameter("Fade", "The fading of the point", 1, 0, 1);
	extendNum = paramsContainer->addIntParameter("Num Props", "The number of props the point is navigating through", 1, 1);
	invertEvens = paramsContainer->addBoolParameter("Invert Evens", "If checked, swap the direction of props with even IDs", false);
	invertOdds = paramsContainer->addBoolParameter("Invert Odds", "If checked, swap the direction of props with odd IDs", false);
}

void RangePattern::getColorsInternal(Array<Colour>* result, Prop* p, double time, int id, int resolution, var params)
{
	var colorVar = getParamValue<var>(color, params);
	Colour bColor = Colour::fromFloatRGBA(colorVar[0], colorVar[1], colorVar[2], colorVar[3]);

	var bgColorVar = getParamValue<var>(bgColor, params);
	Colour bBGColor = Colour::fromFloatRGBA(bgColorVar[0], bgColorVar[1], bgColorVar[2], bgColorVar[3]);

	double bBrightness = getParamValue<double>(brightness, params);
	double bStart = getParamValue<double>(start, params);
	double bEnd = getParamValue<double>(end, params);
	//double bFade = getParamValue<double>(fade, params);
	int bExtend = getParamValue<int>(extendNum, params);
	bool bInvertEvens = getParamValue<bool>(invertEvens, params);
	bool bInvertOdds = getParamValue<bool>(invertOdds, params);

	double extendStart = bStart * bExtend;
	double extendEnd = bEnd * bExtend;

	double relExtStart = (extendStart - id % bExtend) * resolution;
	double relExtEnd = (extendEnd - id % bExtend) * resolution;
	double relStart = jmax<int>(relExtStart, 0);
	double relEnd = jmin<int>(relExtEnd, resolution);

	result->fill(bBGColor.withMultipliedBrightness(bBrightness));


	for (int i = relStart; i <= relEnd && i < resolution; i++)
	{
		bool invert = id % 2 == 0 ? bInvertEvens : bInvertOdds;

		result->set(invert ? resolution - i : i, bColor.withMultipliedBrightness(bBrightness));
		//float diff = 1 - (fabsf(i - relPos) * 1.f / (relSize / (bFade * 2)));
		//bool invert = id % 2 == 0 ? bInvertEvens : bInvertOdds;
		//Colour c = bBGColor.interpolatedWith(bColor, diff);
		//result->set(invert ? resolution - i : i, c.withMultipliedBrightness(bBrightness));
	}

}


MultiPointPattern::MultiPointPattern(var params) :
	PatternBlock(getTypeString(), params)
{
	brightness = paramsContainer->addFloatParameter("Brightness", "", 1, 0, 1);
	color = paramsContainer->addColorParameter("Color", "The color of the point", Colours::white);
	bgColor = paramsContainer->addColorParameter("Background Color", "The color of the background", Colours::black);
	speed = paramsContainer->addFloatParameter("Speed", "The speed between lines per prop", .25f);
	offset = paramsContainer->addFloatParameter("Offset", "Offset of the point", 0);
	gap = paramsContainer->addFloatParameter("Gap", "The gap between lines per prop", .25f, 0);
	size = paramsContainer->addFloatParameter("Size", "Size of the point, relative to the gap", .5f, 0, 1);
	fade = paramsContainer->addFloatParameter("Fade", "The fading of the point", 1, 0, 1);
	extendNum = paramsContainer->addIntParameter("Num Props", "The number of props the point is navigating through", 1, 1);
}

void MultiPointPattern::getColorsInternal(Array<Colour>* result, Prop* p, double time, int id, int resolution, var params)
{
	var colorVar = getParamValue<var>(color, params);
	Colour bColor = Colour::fromFloatRGBA(colorVar[0], colorVar[1], colorVar[2], colorVar[3]);

	var bgColorVar = getParamValue<var>(bgColor, params);
	Colour bBGColor = Colour::fromFloatRGBA(bgColorVar[0], bgColorVar[1], bgColorVar[2], bgColorVar[3]);

	double bBrightness = getParamValue<double>(brightness, params);
	double bOffset = getParamValue<double>(offset, params);
	double bSpeed = getParamValue<double>(speed, params);
	int bExtend = getParamValue<int>(extendNum, params);

	double targetPos = bSpeed * time + bOffset + id * bExtend;
	double bSize = getParamValue<double>(size, params);
	double bFade = getParamValue<double>(fade, params);
	double bGap = getParamValue<double>(gap, params);


	result->fill(bBGColor.withMultipliedBrightness(bBrightness));
	if (bGap == 0 || bSize == 0) return;

	if (targetPos < 0) targetPos = fmod(targetPos, -bGap) + bGap;

	for (int i = 0; i < resolution; i++)
	{
		double relTotal = fmod((1 - (i * 1.0f / resolution)), 1);
		double relGap = fmod((relTotal + bGap + targetPos) / bGap, 1);
		double relCentered = 1 - fabsf((relGap - .5f) * 2) * 1 / bSize;

		if (relCentered < 0) continue;

		double relFadedVal = jmap<double>(jlimit<double>(0, 1, relCentered), 1 - bFade, 1);

		Colour c = bBGColor.interpolatedWith(bColor, relFadedVal);
		result->set(i, c.withMultipliedBrightness(bBrightness));
	}
}

LedRangePattern::LedRangePattern(var params) :
	PatternBlock(getTypeString(), params)
{
	start = paramsContainer->addIntParameter("Start", "Range start", 1);
	count = paramsContainer->addIntParameter("Count", "Number of leds to show", 1);
	color = paramsContainer->addColorParameter("Color", "The color of the leds", Colours::red);
	bgColor = paramsContainer->addColorParameter("Background Color", "The color of the background", Colours::black);
}

void LedRangePattern::getColorsInternal(Array<Colour>* result, Prop* p, double time, int id, int resolution, var params)
{
	int bStart = getParamValue<int>(start, params);
	int bCount = getParamValue<int>(count, params);
	var colorVar = getParamValue<var>(color, params);
	Colour bColor = Colour::fromFloatRGBA(colorVar[0], colorVar[1], colorVar[2], colorVar[3]);
	var bgColorVar = getParamValue<var>(bgColor, params);
	Colour bBGColor = Colour::fromFloatRGBA(bgColorVar[0], bgColorVar[1], bgColorVar[2], bgColorVar[3]);

	for (int i = 0; i < resolution; i++)
	{
		bool isInRange = i >= (bStart-1) && i < (bStart + bCount-1);
		result->set(i, isInRange ? bColor : bBGColor);
	}
}
