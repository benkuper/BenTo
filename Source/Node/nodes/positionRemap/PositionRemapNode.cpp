/*
  ==============================================================================

	PositionRemapNode.cpp
	Created: 13 Apr 2018 11:30:20pm
	Author:  Ben

  ==============================================================================
*/

PositionRemapNode::PositionRemapNode(var params) :
	ColorNode(getTypeString(), params)
{
	position = addFloatParameter("Position", "Center position of the color input", .25f, 0, 1);
	size = addFloatParameter("Size", "Size of the color input", .5f, 0, 1);
	extendNum = addIntParameter("Num Props", "The number of props to extend to", 1,1, 20);
	invertEvens = addBoolParameter("Invert Evens", "If checked, swap the direction of props with even IDs", false);
	invertOdds = addBoolParameter("Invert Odds", "If checked, swap the direction of props with odd IDs", false); 
	bgColor = addColorParameter("Background Color", "The color behind", Colours::transparentBlack);

	mode = addEnumParameter("Mode", "Remap mode");
	mode->addOption("Resize", RESIZE)->addOption("Crop", CROP);

	inColors = addColorSlot(true, "Colors");
	positionSlot = addParameterSlot(true, position);
	sizeSlot = addParameterSlot(true, size);
	extendNumSlot = addParameterSlot(true, extendNum);
	invertEvensSlot = addParameterSlot(true, invertEvens);
	invertOddsSlot = addParameterSlot(true, invertOdds);
	bgColorSlot = addParameterSlot(true, bgColor);
}

PositionRemapNode::~PositionRemapNode()
{
}

Array<Colour> PositionRemapNode::getColorsInternal(Prop * p, double time, var params, var localParams)
{
	int id = params.getProperty("forceID", p->globalID->intValue());
	int resolution = p->resolution->intValue();
	float bPosition = (float)getParameterValue(position, localParams);
	float bSize = (float)getParameterValue(size, localParams);
	int bExtend = jmax((int)getParameterValue(extendNum, localParams), 1);
	float bInvertEvens = (bool)getParameterValue(invertEvens, localParams);
	float bInvertOdds = (bool)getParameterValue(invertOdds, localParams);

	var bgColorVar = getParameterValue(bgColor, localParams);
	Colour bBGColor = Colour::fromFloatRGBA(bgColorVar[0], bgColorVar[1], bgColorVar[2], bgColorVar[3]);

	
	Array<Colour> result = ColorNode::getColorsInternal(p, time, params, localParams);
	Array<Colour> c = inColors->getColors(p, time, params);

	RemapMode m = mode->getValueDataAsEnum<RemapMode>();

	int relId = jmax(id % bExtend,0);
	float relPos = (bPosition * bExtend) - relId;
	float relSize = bSize * bExtend;

	int startLed = (relPos - relSize / 2) * resolution;
	int endLed = (relPos + relSize / 2) * resolution;

	for (int i = 0; i < resolution; i++)
	{
		Colour col = bBGColor;
		if (i >= startLed && i < endLed)
		{
			int tLed = m == CROP ? i : jmap<float>(i, startLed, endLed, 0, resolution);
			col = c[tLed];
		}

		bool invert = id % 2 == 0 ? bInvertEvens : bInvertOdds;
		result.set(invert ? resolution -1- i : i, col);
	}
	return result;
}
