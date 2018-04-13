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

Array<WeakReference<Parameter>> ScriptBlock::getModelParameters()
{
	return script.scriptParamsContainer.getAllParameters();
}

void ScriptBlock::updateColorsForBlock(LightBlock * block, var params)
{
	Array<var> args;
	args.add(block->prop->getScriptObject());
	Array<WeakReference<Parameter>> blockParams = block->paramsContainer.getAllParameters();
	for (auto &p : blockParams) args.add(p->value);
	script.callFunction(updateColorsFunc, args);
}

void ScriptBlock::onControllableFeedbackUpdateInternal(ControllableContainer * cc, Controllable *)
{
	if (cc == &script) providerListeners.call(&ProviderListener::providerParametersChanged, this);
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
		break;
	}
}
