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
	removeChildControllableContainer(paramsContainer.get());
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

void ScriptBlock::getColorsInternal(Array<Colour> * result, Prop * p, double time, int id, int resolution, var params)
{

	ColourScriptData cData;
	cData.colorArray.resize(resolution);

	Array<var> args;
	args.add(cData.getScriptObject());
	args.add(id);
	args.add(resolution);
	args.add(time);
	args.add(params);

	var scriptResult = script.callFunction(updateColorsFunc, args); 
	
	Array<Colour> colors;

	if (scriptResult.isArray())
	{

		colors.resize(resolution);
		int numColors = jmin<int>(resolution, scriptResult.size());
		for (int i = 0; i < numColors; i++)
		{
			colors.set(i, Colour::fromFloatRGBA((float)scriptResult[i][0], 
										   (float)scriptResult[i][1], 
				                           (float)scriptResult[i][2], 
				                           scriptResult[i].size() > 3? (float)scriptResult[i][3]:1));
		}
	}
	else
	{
		result->swapWith(cData.colorArray);
	}
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
	p->colorArray.set(index, Colour::fromFloatRGBA((float)args.arguments[1], (float)args.arguments[2], (float)args.arguments[3], args.numArguments > 4?(float)args.arguments[4]:1));


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
	p->colorArray.set(index, Colour::fromHSV((float)args.arguments[1], (float)args.arguments[2], (float)args.arguments[3], args.numArguments > 4 ? (float)args.arguments[4] : 1));

	
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
		p->colorArray.set(i, Colour::fromFloatRGBA((float)colors[i][0], (float)colors[i][1], (float)colors[i][2], args.numArguments > 3 ? (float)args.arguments[3] : 1));
	}

	return colors;
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
		p->colorArray.set(i, Colour::fromHSV((float)colors[i][0], (float)colors[i][1], (float)colors[i][2], args.numArguments > 3 ? (float)args.arguments[3] : 1));
	}

	return colors;
}
