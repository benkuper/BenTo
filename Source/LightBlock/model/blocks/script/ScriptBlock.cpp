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
}

ScriptBlock::~ScriptBlock()
{
}

Array<WeakReference<Controllable>> ScriptBlock::getModelParameters()
{
	return script.scriptParamsContainer.getAllControllables();
}

Array<Colour> ScriptBlock::getColors(int id, int resolution, float time, var params)
{
	ColourScriptData colors(resolution);

	Array<var> args;
	args.add(colors.getScriptObject());
	args.add(id);
	args.add(resolution);
	args.add(time);
	args.add(params);

	script.callFunction(updateColorsFunc, args);

	return Array<Colour>(colors.colorArray);
}

void ScriptBlock::onControllableFeedbackUpdateInternal(ControllableContainer * cc, Controllable * c)
{
	if (cc == &script) providerListeners.call(&ProviderListener::providerParameterValueUpdated, this, dynamic_cast<Parameter *>(c));
}

void ScriptBlock::childStructureChanged(ControllableContainer * cc)
{
	if (cc == &script) providerListeners.call(&ProviderListener::providerParametersChanged, this);
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
		break;
	}
}

ColourScriptData::ColourScriptData(int resolution) :
	ScriptTarget("colours",this)
{
	Array<Colour> colors;
	colors.resize(resolution);
	scriptObject.setMethod("setRGB", &ColourScriptData::updateColorRGBFromScript);
	scriptObject.setMethod("setHSV", &ColourScriptData::updateColorHSVFromScript);
}


//COLOUR SCRIPT DATA

var ColourScriptData::updateColorRGBFromScript(const var::NativeFunctionArgs & args)
{
	ColourScriptData * p = getObjectFromJS<ColourScriptData>(args);
	if (p == nullptr) return var();
	if (args.numArguments < 4)
	{
		LOGERROR("SetColor RGB from script not enough parameters");
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
		LOGERROR("SetColor HSV from script not enough parameters");
		return var();
	}
	int index = args.arguments[0];
	p->colorArray.set(index, Colour::fromHSV((float)args.arguments[1], (float)args.arguments[2], (float)args.arguments[3], 1));

	
	return var();
}