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
	PatternBlock(const String &name = "Pattern", var params = var());
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

	FloatParameter * density;
	FloatParameter * offset;
	FloatParameter * speed;
	FloatParameter * idOffset;

	Array<Colour> getColors(int id, int resolution, float time, var params) override;

	String getTypeString() const override { return "Rainbow"; }
};

class NoisePattern :
	public PatternBlock
{
public:
	NoisePattern(var params = var());
	~NoisePattern() {}

	siv::PerlinNoise perlin;

	FloatParameter * scale;
	FloatParameter * speed;
	FloatParameter * contrast;
	FloatParameter * brightness;
	ColorParameter * color;
	FloatParameter * idOffset;

	Array<Colour> getColors(int id, int resolution, float time, var params) override;

	String getTypeString() const override { return "Noise"; }
};

class SolidColorPattern :
	public PatternBlock
{
public:
	SolidColorPattern(var params = var());
	~SolidColorPattern() {}

	ColorParameter * color;
	FloatParameter * hueSpeed;
	FloatParameter * idOffset;

	Array<Colour> getColors(int id, int resolution, float time, var params) override;

	String getTypeString() const override { return "Solid Color"; }
};

class StrobePattern :
	public PatternBlock
{
public:
	StrobePattern(var params = var());
	~StrobePattern() {}

	ColorParameter * color;
	FloatParameter * frequency;
	FloatParameter * onOffBalance;
	FloatParameter * idOffset;

	Array<Colour> getColors(int id, int resolution, float time, var params) override;

	String getTypeString() const override { return "Strobe"; }
};