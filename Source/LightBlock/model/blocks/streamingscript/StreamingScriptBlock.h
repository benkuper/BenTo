/*
  ==============================================================================

    StreamingScriptBlock.h
    Created: 10 Apr 2018 6:59:13pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

class ColourScriptData :
	public ScriptTarget
{
public:
	ColourScriptData(Array<Colour> * sourceColors);

	Array<Colour> colorArray;
	Array<Colour>* sourceColors;

	static var updateColorRGBFromScript(const var::NativeFunctionArgs &args);
	static var updateColorHSVFromScript(const var::NativeFunctionArgs &args);
	static var updateColorsRGBFromScript(const var::NativeFunctionArgs &args);
	static var updateColorsHSVFromScript(const var::NativeFunctionArgs &args);

	static var fillRGBFromScript(const var::NativeFunctionArgs& args);
	static var fillHSVFromScript(const var::NativeFunctionArgs& args);
	static var pointRGBFromScript(const var::NativeFunctionArgs& args);
	static var pointHSVFromScript(const var::NativeFunctionArgs& args);
	static var gradientRGBFromScript(const var::NativeFunctionArgs& args);
	static var gradientHSVFromScript(const var::NativeFunctionArgs& args);
	static var lerpColorFromScript(const var::NativeFunctionArgs& args);
	static var getHSVColorFromScript(const var::NativeFunctionArgs& args);

	static Colour getColorFromArgs(const var * a, int numArgs, int offset, bool useHSV = false);

	static var getColorFromScript(const var::NativeFunctionArgs& args);
	static var getArrayFromScript(const var::NativeFunctionArgs& args);

};

class StreamingScriptBlock :
	public LightBlockModel,
	public Script::AsyncListener
{
public:
	StreamingScriptBlock(var params = var());
	~StreamingScriptBlock();

	const Identifier updateColorsFunc = "updateColors";
	Script script;

	virtual Array<WeakReference<Controllable>> getModelParameters() override;

	void getColorsInternal(Array<Colour> * result, Prop * p, double time, int id, int resolution, var params) override;

	void onControllableFeedbackUpdateInternal(ControllableContainer * cc, Controllable *) override;
	void childStructureChanged(ControllableContainer * cc) override;

	void handleEnterExit(bool enter, Array<Prop*> props) override;

	var getJSONData() override;
	void loadJSONDataInternal(var data) override;
	
	void newMessage(const Script::ScriptEvent &e) override;

	DECLARE_TYPE("Streaming Script")

};