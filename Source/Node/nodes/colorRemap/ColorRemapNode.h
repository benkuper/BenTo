/*
  ==============================================================================

    ColorRemapNode.h
    Created: 13 Apr 2018 11:29:57pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

class ColorRemapNode :
	public ColorNode
{
public:
	ColorRemapNode(var params = var());
	~ColorRemapNode();

	ColorSlot* c1;
	GradientColorManager colorManager;

	enum InputMode { HUE, SATURATION, BRIGHTNESS};
	EnumParameter* inputModeParam;

	Array<Colour> getColorsInternal(Prop* p, double time, var params, var localParams) override;

	String getTypeString() const override { return "Color Remap"; }
	static ColorRemapNode* create(var params) { return new ColorRemapNode(params); }
};