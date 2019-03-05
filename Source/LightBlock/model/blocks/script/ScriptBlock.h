/*
  ==============================================================================

    ScriptBlock.h
    Created: 10 Apr 2018 6:59:13pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "../../LightBlockModel.h"

class ColourScriptData :
	public ScriptTarget
{
public:
	ColourScriptData();

	Array<Colour> colorArray;

	static var updateColorRGBFromScript(const var::NativeFunctionArgs &args);
	static var updateColorHSVFromScript(const var::NativeFunctionArgs &args);
	static var updateColorsRGBFromScript(const var::NativeFunctionArgs &args);
	static var updateColorsHSVFromScript(const var::NativeFunctionArgs &args);
};

class ScriptBlock :
	public LightBlockModel,
	public Script::AsyncListener
{
public:
	ScriptBlock(var params = var());
	~ScriptBlock();

	const Identifier updateColorsFunc = "updateColors";
	Script script;

	virtual Array<WeakReference<Controllable>> getModelParameters() override;

	void getColorsInternal(Array<Colour> * result, Prop * p, double time, int id, int resolution, var params) override;

	void onControllableFeedbackUpdateInternal(ControllableContainer * cc, Controllable *) override;
	void childStructureChanged(ControllableContainer * cc) override;

	var getJSONData() override;
	void loadJSONDataInternal(var data) override;
	
	void newMessage(const Script::ScriptEvent &e) override;

	String getTypeString() const override { return "Script"; }
	static ScriptBlock * create(var params) { return new ScriptBlock(params); }
};