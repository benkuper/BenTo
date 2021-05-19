/*
  ==============================================================================

	PatternBlock.h
	Created: 10 Apr 2018 6:58:57pm
	Author:  Ben

  ==============================================================================
*/

#pragma once

#include "../../LightBlockModel.h"
#include "PerlinNoise.hpp"

class PatternBlock :
	public LightBlockModel
{
public:
	PatternBlock(const String& name = "Pattern", var params = var());
	~PatternBlock();

	String getTypeString() const override { return "Pattern"; }
};


//Patterns

class RainbowPattern :
	public PatternBlock
{
public:
	RainbowPattern(var params = var());
	~RainbowPattern() {}

	FloatParameter* density;
	FloatParameter* offset;
	FloatParameter* speed;
	FloatParameter* idOffset;
	FloatParameter* brightness;

	void getColorsInternal(Array<Colour>* result, Prop* p, double time, int id, int resolution, var params) override;

	String getTypeString() const override { return "Rainbow"; }
};

class NoisePattern :
	public PatternBlock
{
public:
	NoisePattern(var params = var());
	~NoisePattern() {}

	siv::PerlinNoise perlin;

	FloatParameter* brightness;
	FloatParameter* scale;
	FloatParameter* speed;
	FloatParameter* contrast;
	FloatParameter* balance;
	ColorParameter* color;
	ColorParameter* bgColor;
	FloatParameter* idOffset;

	void getColorsInternal(Array<Colour>* result, Prop* p, double time, int id, int resolution, var params) override;

	String getTypeString() const override { return "Noise"; }
};

class SolidColorPattern :
	public PatternBlock
{
public:
	SolidColorPattern(var params = var());
	~SolidColorPattern() {}

	ColorParameter* color;
	FloatParameter* brightness;
	FloatParameter* hueSpeed;
	FloatParameter* idOffset;

	void getColorsInternal(Array<Colour>* result, Prop* p, double time, int id, int resolution, var params) override;

	String getTypeString() const override { return "Solid Color"; }
};

class StrobePattern :
	public PatternBlock
{
public:
	StrobePattern(var params = var());
	~StrobePattern() {}

	FloatParameter* brightness;
	ColorParameter* color;
	ColorParameter* color2;
	FloatParameter* frequency;
	FloatParameter* onOffBalance;
	FloatParameter* offset;
	FloatParameter* idOffset;
	FloatParameter* fadeIn;
	FloatParameter* fadeOut;

	void getColorsInternal(Array<Colour>* result, Prop* p, double time, int id, int resolution, var params) override;

	String getTypeString() const override { return "Strobe"; }
};


class PointPattern :
	public PatternBlock
{
public:
	PointPattern(var params = var());
	~PointPattern() {}

	FloatParameter* brightness;
	FloatParameter* position;
	FloatParameter* size;
	FloatParameter* fade;
	ColorParameter* color;
	ColorParameter* bgColor;
	IntParameter* extendNum;
	BoolParameter* invertOdds;
	BoolParameter* invertEvens;

	void getColorsInternal(Array<Colour>* result, Prop* p, double time, int id, int resolution, var params) override;

	String getTypeString() const override { return "Point"; }
};

class MultiPointPattern :
	public PatternBlock
{
public:
	MultiPointPattern(var params = var());
	~MultiPointPattern() {}

	FloatParameter* brightness;
	FloatParameter* offset;
	FloatParameter* speed;
	FloatParameter* gap;
	FloatParameter* size;
	FloatParameter* fade;
	ColorParameter* color;
	ColorParameter* bgColor;
	IntParameter* extendNum;

	void getColorsInternal(Array<Colour>* result, Prop* p, double time, int id, int resolution, var params) override;

	String getTypeString() const override { return "Multipoint"; }
};
