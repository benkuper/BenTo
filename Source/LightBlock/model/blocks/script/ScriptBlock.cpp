/*
  ==============================================================================

    ScriptBlock.cpp
    Created: 10 Apr 2018 6:59:13pm
    Author:  Ben

  ==============================================================================
*/

#include "ScriptBlock.h"
#include "Prop/Prop.h"

ScriptBlock::ScriptBlock(var params) :
	LightBlockModel(getTypeString(), params),
	script(this)
{
	removeChildControllableContainer(paramsContainer);
	removeChildControllableContainer(&presetManager);
	addChildControllableContainer(&script);
	addChildControllableContainer(&presetManager);

	script.addAsyncScriptListener(this);

	//colorDataObject = colorData.getScriptObject();
}

ScriptBlock::~ScriptBlock()
{
}

Array<WeakReference<Controllable>> ScriptBlock::getModelParameters()
{
	return script.scriptParamsContainer.getAllControllables();
}

Array<Colour> ScriptBlock::getColors(Prop * p, double time, var params)
{
	int resolution = p->resolution->intValue();
	int id = params.getProperty("forceID", p->globalID->intValue());

	ColourScriptData cData;
	cData.colorArray.resize(resolution);

	Array<var> args;
	args.add(cData.getScriptObject());
	args.add(id);
	args.add(resolution);
	args.add(time);
	args.add(params);

	var result = script.callFunction(updateColorsFunc, args); 
	
	Array<Colour> colors;

	if (result.isArray())
	{

		colors.resize(resolution);
		int numColors = jmin<int>(resolution, result.size());
		for (int i = 0; i < numColors; i++)
		{
			colors.set(i, Colour::fromRGB((float)result[i][0] * 255, (float)result[i][1] * 255, (float)result[i][2] * 255));
		}
	}
	else
	{
		colors = Array<Colour>(cData.colorArray);
	}
	
	
	return colors;
}

void ScriptBlock::onControllableFeedbackUpdateInternal(ControllableContainer * cc, Controllable * c)
{
	if (cc == &script && script.state == Script::SCRIPT_LOADED) providerListeners.call(&ProviderListener::providerParameterValueUpdated, this, dynamic_cast<Parameter *>(c));
}

void ScriptBlock::childStructureChanged(ControllableContainer * cc)
{
	if (cc == &script && script.state == Script::SCRIPT_LOADED) providerListeners.call(&ProviderListener::providerParametersChanged, this);
}

var ScriptBlock::getJSONData()
{
	var data = LightBlockModel::getJSONData();
	data.getDynamicObject()->setProperty("script", script.getJSONData());
	return data;
}

void ScriptBlock::loadJSONDataInternal(var data)
{
	script.loadJSONData(data.getProperty("script", var()));
	setCustomThumbnail(script.filePath->getFile().withFileExtension("png").getFullPathName());
	LightBlockModel::loadJSONDataInternal(data);

}

void ScriptBlock::newMessage(const Script::ScriptEvent & e)
{
	switch (e.type)
	{
	case Script::ScriptEvent::STATE_CHANGE:
		setCustomThumbnail(script.filePath->getFile().withFileExtension("png").getFullPathName());
		setNiceName(script.niceName);

		if(script.state == Script::SCRIPT_LOADED) providerListeners.call(&ProviderListener::providerParametersChanged, this);
		break;
	}
}

ColourScriptData::ColourScriptData() :
	ScriptTarget("colours",this)
{
	scriptObject.setMethod("setRGB", &ColourScriptData::updateColorRGBFromScript);
	scriptObject.setMethod("setHSV", &ColourScriptData::updateColorHSVFromScript);
	scriptObject.setMethod("setColorsRGB", &ColourScriptData::updateColorsRGBFromScript);
	scriptObject.setMethod("setColorsHSV", &ColourScriptData::updateColorsHSVFromScript);
}


//COLOUR SCRIPT DATA

var ColourScriptData::updateColorRGBFromScript(const var::NativeFunctionArgs & args)
{
	ColourScriptData * p = getObjectFromJS<ColourScriptData>(args);
	if (p == nullptr) return var();
	if (args.numArguments < 4)
	{
		LOGWARNING("SetColor RGB from script not enough parameters");
		return var();
	}
	int index = args.arguments[0];
	p->colorArray.set(index, Colour::fromRGB((float)args.arguments[1] * 255, (float)args.arguments[2] * 255, (float)args.arguments[3] * 255));


	return var();
}

var ColourScriptData::updateColorHSVFromScript(const var::NativeFunctionArgs & args)
{
	ColourScriptData * p = getObjectFromJS<ColourScriptData>(args);
	if (p == nullptr) return var();
	if (args.numArguments < 4)
	{
		LOGWARNING("SetColor HSV from script not enough parameters");
		return var();
	}
	int index = args.arguments[0];
	p->colorArray.set(index, Colour::fromHSV((float)args.arguments[1], (float)args.arguments[2], (float)args.arguments[3], 1));

	
	return var();
}

var ColourScriptData::updateColorsRGBFromScript(const var::NativeFunctionArgs & args)
{
	ColourScriptData * p = getObjectFromJS<ColourScriptData>(args);
	if (p == nullptr) return var();
	if (args.numArguments == 0 || !args.arguments[0].isArray())
	{
		LOGWARNING("set colors RGB needs one array argument.");
		return var();
	}

	var colors = args.arguments[0];
	int numColors = jmin<int>(p->colorArray.size(), colors.size());
	for (int i = 0; i < numColors; i++)
	{
		p->colorArray.set(i, Colour::fromRGB((float)colors[i][0] * 255, (float)colors[i][1] * 255, (float)colors[i][2] * 255));
	}

}

var ColourScriptData::updateColorsHSVFromScript(const var::NativeFunctionArgs & args)
{
	ColourScriptData * p = getObjectFromJS<ColourScriptData>(args);
	if (p == nullptr) return var();
	if (args.numArguments == 0 || !args.arguments[0].isArray())
	{
		LOGWARNING("set colors HSV needs one array argument.");
		return var();
	}

	var colors = args.arguments[0];
	int numColors = jmin<int>(p->colorArray.size(), colors.size());
	for (int i = 0; i < numColors; i++)
	{
		p->colorArray.set(i, Colour::fromHSV((float)colors[i][0] * 255, (float)colors[i][1] * 255, (float)colors[i][2] * 255, 1));
	}
}
