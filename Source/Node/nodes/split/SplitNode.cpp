/*
  ==============================================================================

	SplitNode.cpp
	Created: 20 Oct 2023 11:40:50am
	Author:  bkupe

  ==============================================================================
*/

#include "Node/NodeIncludes.h"
#include "Prop/PropIncludes.h"

SplitNode::SplitNode(var params) :
	ColorNode(getTypeString(), params)
{
	for (int i = 0; i < 3; i++)
	{
		positions[i] = addFloatParameter("Position " + String(i + 1), "Center position of the color input", .5f, 0, 1);
	}

	fade = addFloatParameter("Fade", "Fade between splits", .5f, 0, 2);

	for (int i = 0; i < 4; i++) inColors[i] = addColorSlot(true, "Colors " + String(i + 1));
	for (int i = 0; i < 3; i++) positionSlots[i] = addParameterSlot(true, positions[i]);
	fadeSlot = addParameterSlot(true, fade);
}

SplitNode::~SplitNode()
{
}

Array<Colour> SplitNode::getColorsInternal(Prop* p, double time, var params, var localParams)
{
	//int id = params.getProperty("forceID", p->globalID->intValue());
	int resolution = p->getResolution();

	float bPositions[3];
	for (int i = 0; i < 3; i++) bPositions[i] = inColors[i + 1]->isConnected() ? (float)getParameterValue(positions[i], params, localParams) : -1;

	//float bFade = (float)getParameterValue(fade, params, localParams);

	Array<Colour> result = ColorNode::getColorsInternal(p, time, params, localParams);
	Array<Array<Colour>> colors;
	for (int i = 0; i < 4; i++)
	{
		if (!inColors[i]->isConnected()) continue;
		colors.set(i, inColors[i]->getColors(p, time, params));
	}

	for (int i = 0; i < 4; i++)
	{
		float startPos = i == 0 ? 0 : bPositions[i - 1];
		float endPos = i < (4 - 1) ? bPositions[i] : 1;
		if (startPos < 0) continue;
		if (endPos < 0) endPos = 1;

		int startLed = startPos * resolution;
		int endLed = endPos * resolution;
		for (int j = startLed; j < endLed; j++)
		{
			result.set(j, colors[i][j]); //crop mode
		}
	}



	return result;
}
