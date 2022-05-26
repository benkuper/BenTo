/*
  ==============================================================================

	ScriptBlock.cpp
	Created: 10 Apr 2018 6:59:13pm
	Author:  Ben

  ==============================================================================
*/

ScriptBlock::ScriptBlock(var params) :
	LightBlockModel(getTypeString(), params),
	script(this)
{
	itemDataType = getTypeString();

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
	return script.scriptParamsContainer->getAllControllables();
}

void ScriptBlock::getColorsInternal(Array<Colour>* result, Prop* p, double time, int id, int resolution, var params)
{

	ColourScriptData cData(result);
	cData.colorArray.resize(resolution);

	Array<var> args;
	args.add(cData.getScriptObject());
	args.add(id);
	args.add(resolution);
	args.add(time);
	args.add(params);
	args.add(p->getScriptObject());

	var scriptResult = script.callFunction(updateColorsFunc, args);

//	Array<Colour> colors;
	if (scriptResult.isArray())
	{

		//colors.resize(resolution);
		int numColors = jmin<int>(resolution, scriptResult.size());
		for (int i = 0; i < numColors; i++)
		{
			result->set(i, Colour::fromFloatRGBA((float)scriptResult[i][0],
				(float)scriptResult[i][1],
				(float)scriptResult[i][2],
				scriptResult[i].size() > 3 ? (float)scriptResult[i][3] : 1));
		}

	}
	else
	{
		result->swapWith(cData.colorArray);
	}
}

void ScriptBlock::onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c)
{
	if (cc == &script && script.state == Script::SCRIPT_LOADED) providerListeners.call(&ProviderListener::providerParameterValueUpdated, this, dynamic_cast<Parameter*>(c));
}

void ScriptBlock::childStructureChanged(ControllableContainer* cc)
{
	if (cc == &script && script.state == Script::SCRIPT_LOADED) providerListeners.call(&ProviderListener::providerParametersChanged, this);
}

void ScriptBlock::handleEnterExit(bool enter, Array<Prop *> props)
{
	Array<var> args;
	var propArgs;
	for (auto& p : props) propArgs.append(p->getScriptObject());
	args.add(propArgs);
	script.callFunction(enter ? "onEnter" : "onExit", args);
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

void ScriptBlock::newMessage(const Script::ScriptEvent& e)
{
	switch (e.type)
	{
	case Script::ScriptEvent::STATE_CHANGE:
		setCustomThumbnail(script.filePath->getFile().withFileExtension("png").getFullPathName());
		setNiceName(script.niceName);

		if (script.state == Script::SCRIPT_LOADED) providerListeners.call(&ProviderListener::providerParametersChanged, this);
		break;
	}
}

//COLOUR SCRIPT DATA
ColourScriptData::ColourScriptData(Array<Colour>* sourceColors) :
	ScriptTarget("colours", this),
	sourceColors(sourceColors)
{
	scriptObject.setMethod("getArray", &ColourScriptData::getArrayFromScript);
	scriptObject.setMethod("get", &ColourScriptData::getColorFromScript);
	scriptObject.setMethod("set", &ColourScriptData::updateColorRGBFromScript);
	scriptObject.setMethod("setHSV", &ColourScriptData::updateColorHSVFromScript);
	scriptObject.setMethod("setColors", &ColourScriptData::updateColorsRGBFromScript);
	scriptObject.setMethod("setColorsHSV", &ColourScriptData::updateColorsHSVFromScript);
	scriptObject.setMethod("fill", &ColourScriptData::fillRGBFromScript);
	scriptObject.setMethod("fillHSV", &ColourScriptData::fillHSVFromScript);
	scriptObject.setMethod("point", &ColourScriptData::pointRGBFromScript);
	scriptObject.setMethod("pointHSV", &ColourScriptData::pointHSVFromScript);
	scriptObject.setMethod("gradient", &ColourScriptData::gradientRGBFromScript);
	scriptObject.setMethod("gradientHSV", &ColourScriptData::gradientHSVFromScript);
	scriptObject.setMethod("lerpColor", &ColourScriptData::lerpColorFromScript);
	scriptObject.setMethod("getHSV", &ColourScriptData::getHSVColorFromScript);
}



var ColourScriptData::updateColorRGBFromScript(const var::NativeFunctionArgs& args)
{
	ColourScriptData* p = getObjectFromJS<ColourScriptData>(args);
	if (p == nullptr) return var();
	if (args.numArguments < 2)
	{
		LOGWARNING("SetColor RGB from script not enough parameters");
		return var();
	}
	int index = args.arguments[0];
	Colour c = getColorFromArgs(args.arguments, args.numArguments, 1, false);
	if(index >= 0 && index <= p->colorArray.size()) p->colorArray.set(index, c);
	return var();
}

var ColourScriptData::updateColorHSVFromScript(const var::NativeFunctionArgs& args)
{
	ColourScriptData* p = getObjectFromJS<ColourScriptData>(args);
	if (p == nullptr) return var();
	if (args.numArguments < 2)
	{
		LOGWARNING("SetColor HSV from script not enough parameters");
		return var();
	}
	int index = args.arguments[0];
	Colour c = getColorFromArgs(args.arguments, args.numArguments, 1, true);
	p->colorArray.set(index, c);
	return var();
}

var ColourScriptData::updateColorsRGBFromScript(const var::NativeFunctionArgs& args)
{
	ColourScriptData* p = getObjectFromJS<ColourScriptData>(args);
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
		Colour c = getColorFromArgs(&colors[i], colors[i].size(), 0, false);
		p->colorArray.set(i, c);
	}

	return colors;
}

var ColourScriptData::updateColorsHSVFromScript(const var::NativeFunctionArgs& args)
{
	ColourScriptData* p = getObjectFromJS<ColourScriptData>(args);
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
		Colour c = getColorFromArgs(&colors[i], colors[i].size(), 0, true);
		p->colorArray.set(i, c);
	}

	return colors;
}

var ColourScriptData::fillRGBFromScript(const var::NativeFunctionArgs& args)
{
	ColourScriptData* p = getObjectFromJS<ColourScriptData>(args);
	if (p == nullptr) return var();
	if (args.numArguments == 0) return var();

	Colour c = getColorFromArgs(args.arguments, args.numArguments, 0, false);

	if (args.numArguments < 3 || !args.arguments[0].isArray()) p->colorArray.fill(c);
	else
	{
		float start = (float)args.arguments[1];
		float end = (float)args.arguments[2];

		int resolution = p->colorArray.size();
		int s = jmax(jmin(start, end), 0.f) * (resolution - 1);
		int e = jmin(jmax(start, end), 1.f) * (resolution - 1);

		for (int i = s; i <= e; i++)
		{
			p->colorArray.set(i, c);
		}
	}
	return var();
}

var ColourScriptData::fillHSVFromScript(const var::NativeFunctionArgs& args)
{
	ColourScriptData* p = getObjectFromJS<ColourScriptData>(args);
	if (p == nullptr) return var();
	if (args.numArguments == 0) return var();

	Colour c = getColorFromArgs(args.arguments, args.numArguments, 0, true);

	if (args.numArguments < 3 || !args.arguments[0].isArray()) p->colorArray.fill(c);
	else
	{
		float start = (float)args.arguments[1];
		float end = (float)args.arguments[2];

		int resolution = p->colorArray.size();
		int s = jmax(jmin(start, end), 0.f) * (resolution - 1);
		int e = jmin(jmax(start, end), 1.f) * (resolution - 1);

		for (int i = s; i <= e; i++)
		{
			p->colorArray.set(i, c);
		}
	}
	return var();
}

var ColourScriptData::pointRGBFromScript(const var::NativeFunctionArgs& args)
{
	ColourScriptData* p = getObjectFromJS<ColourScriptData>(args);
	if (p == nullptr) return var();
	if (args.numArguments < 3) return var();

	Colour c = getColorFromArgs(args.arguments, args.numArguments, 0, false);
	float pos = args.arguments[1];
	float radius = args.arguments[2];
	if (radius == 0) return var();

	int resolution = p->colorArray.size();

	for (int i = 0; i < resolution; i++)
	{
		float rel = i * 1.0f / (resolution - 1);
		float fac = jmax(1 - (abs(pos - rel) / radius), 0.f);
		Colour ic = c.withMultipliedAlpha(fac);
		p->colorArray.set(i, p->colorArray[i].overlaidWith(ic));
	}
	return var();
}

var ColourScriptData::pointHSVFromScript(const var::NativeFunctionArgs& args)
{
	ColourScriptData* p = getObjectFromJS<ColourScriptData>(args);
	if (p == nullptr) return var();
	if (args.numArguments < 3) return var();

	Colour c = getColorFromArgs(args.arguments, args.numArguments, 0, true);
	float pos = args.arguments[1];
	float radius = args.arguments[2];
	if (radius == 0) return var();

	int resolution = p->colorArray.size();

	for (int i = 0; i < resolution; i++)
	{
		float rel = i * 1.0f / (resolution - 1);
		float fac = jmax(1 - (abs(pos - rel) / radius), 0.f);
		Colour ic = c.withMultipliedAlpha(fac);
		p->colorArray.set(i, p->colorArray[i].overlaidWith(ic));
	}
	return var();
}


var ColourScriptData::gradientRGBFromScript(const var::NativeFunctionArgs& args)
{
	ColourScriptData* p = getObjectFromJS<ColourScriptData>(args);
	if (p == nullptr) return var();
	if (args.numArguments < 2 || !args.arguments[0].isArray() || !args.arguments[1].isArray()) return var();

	Colour c1 = getColorFromArgs(args.arguments, args.numArguments, 0, false);
	Colour c2 = getColorFromArgs(args.arguments, args.numArguments, 1, false);

	float start = args.numArguments > 3 ? (float)args.arguments[2] : 0;
	float end = args.numArguments > 3 ? (float)args.arguments[3] : 1;

	int resolution = p->colorArray.size();
	int s = jmax(jmin(start, end), 0.f) * (resolution - 1);
	int e = jmin(jmax(start, end), 1.f) * (resolution - 1);

	for (int i = s; i <= e; i++)
	{
		float rel = (i - s) * 1.0f / (e - s);
		p->colorArray.set(i, c1.interpolatedWith(c2, rel));
	}
	return var();
}

var ColourScriptData::gradientHSVFromScript(const var::NativeFunctionArgs& args)
{
	ColourScriptData* p = getObjectFromJS<ColourScriptData>(args);
	if (p == nullptr) return var();
	if (args.numArguments < 2 || !args.arguments[0].isArray() || !args.arguments[1].isArray()) return var();

	Colour c1 = getColorFromArgs(args.arguments, args.numArguments, 0, true);
	Colour c2 = getColorFromArgs(args.arguments, args.numArguments, 1, true);

	float start = args.numArguments > 3 ? (float)args.arguments[2] : 0;
	float end = args.numArguments > 3 ? (float)args.arguments[3] : 1;

	int resolution = p->colorArray.size();
	int s = jmax(jmin(start, end), 0.f) * (resolution - 1);
	int e = jmin(jmax(start, end), 1.f) * (resolution - 1);

	for (int i = s; i <= e; i++)
	{
		float rel = (i - s) * 1.0f / (e - s);
		p->colorArray.set(i, c1.interpolatedWith(c2, rel));
	}
	return var();
}

var ColourScriptData::lerpColorFromScript(const var::NativeFunctionArgs& args)
{
	ColourScriptData* p = getObjectFromJS<ColourScriptData>(args);
	if (p == nullptr) return var();
	if (args.numArguments < 3 || !args.arguments[0].isArray() || !args.arguments[1].isArray()) return var();

	Colour c1 = getColorFromArgs(args.arguments, args.numArguments, 0);
	Colour c2 = getColorFromArgs(args.arguments, args.numArguments, 1);

	float rel = (float)args.arguments[2];
	Colour cr =  c1.interpolatedWith(c2, rel);

	var result;
	result.append(cr.getFloatRed());
	result.append(cr.getFloatGreen());
	result.append(cr.getFloatBlue());
	result.append(cr.getFloatAlpha());
	return result;
}


var ColourScriptData::getHSVColorFromScript(const var::NativeFunctionArgs& args)
{
	ColourScriptData* p = getObjectFromJS<ColourScriptData>(args);
	if (p == nullptr) return var();
	if (args.numArguments == 0) return var();

	Colour c = getColorFromArgs(args.arguments, args.numArguments, 0);

	var result;
	result.append(c.getHue());
	result.append(c.getSaturation());
	result.append(c.getBrightness());
	result.append(c.getFloatAlpha());
	return result;
}

Colour ColourScriptData::getColorFromArgs(const var* a, int numArgs, int offset, bool useHSV)
{
	Colour c;
	if (numArgs <= offset) return c;

	if (a[offset].isArray() && a[offset].size() >= 3)
	{
		if (useHSV) c = Colour::fromHSV((float)a[offset][0], (float)a[offset][1], (float)a[offset][2], a[offset].size() > 3 ? (float)a[offset][3] : (numArgs > offset + 1 ? (float)a[offset + 1] : 1));
		else c = Colour::fromFloatRGBA((float)a[offset][0], (float)a[offset][1], (float)a[offset][2], a[offset].size() > 3 ? (float)a[offset][3] : (numArgs > offset + 1 ? (float)a[offset + 1] : 1));
	}
	else if (numArgs >= offset + 3)
	{
		if (useHSV) c = Colour::fromHSV((float)a[offset], (float)a[offset + 1], (float)a[offset + 2], numArgs > offset + 3 ? (float)a[offset + 3] : 1);
		else  c = Colour::fromFloatRGBA((float)a[offset], (float)a[offset + 1], (float)a[offset + 2], numArgs > offset + 3 ? (float)a[offset + 3] : 1);
	}
	return c;
}

var ColourScriptData::getColorFromScript(const var::NativeFunctionArgs& args)
{
	ColourScriptData* p = getObjectFromJS<ColourScriptData>(args);
	if (p == nullptr) return var();
	if (args.numArguments == 0) return var();

	
	int index = args.arguments[0];
	if (p->sourceColors == nullptr || index < 0 || index >= p->sourceColors->size()) return var();

	Colour col = p->sourceColors->getUnchecked(index);
	
	var c;
	c.append(col.getFloatRed());
	c.append(col.getFloatGreen());
	c.append(col.getFloatBlue());
	c.append(col.getFloatAlpha());
	return c;
}

var ColourScriptData::getArrayFromScript(const var::NativeFunctionArgs& args)
{
	ColourScriptData* p = getObjectFromJS<ColourScriptData>(args);
	if (p == nullptr) return var();
	if (p->sourceColors == nullptr) return var();

	var result;
	for (auto& col : *p->sourceColors)
	{
		var c;
		c.append(col.getFloatRed());
		c.append(col.getFloatGreen());
		c.append(col.getFloatBlue());
		c.append(col.getFloatAlpha());

		result.append(c);
	}
	
	return result;
}
