/*
  ==============================================================================

	LightBlockLayer.cpp
	Created: 17 Apr 2018 5:10:36pm
	Author:  Ben

  ==============================================================================
*/

#include "LightBlockLayer.h"
#include "ui/LightBlockLayerPanel.h"
#include "ui/LightBlockLayerTimeline.h"

LightBlockLayer::LightBlockLayer(Sequence * s, var) :
	SequenceLayer(s, "Block Layer"),
	blockClipManager(this)
{
	defaultLayer = addBoolParameter("Default", "If checked, this layer will be the default layer when no layer has the requested prop id", false);
	globalLayer = addBoolParameter("Global", "If checked, this layer will always be used in addition to other potential layers", false);
	targetId = addIntParameter("Prop ID", "Target Prop ID to assign this layer to", 0, 0, INT32_MAX);
}

LightBlockLayer::~LightBlockLayer()
{
}


Array<Colour> LightBlockLayer::getColors(int id, int resolution, double time, var params)
{
	Array<LightBlockClip *> clips = blockClipManager.getClipsAtTime(time);

	Array<Colour> result;
	result.resize(resolution);

	if (clips.size() == 0)
	{
		result.fill(Colours::black);
		return result;
	}

	Array<Array<Colour>> clipColors;

	for (auto &c : clips)
	{
		clipColors.add(c->getColors(id, resolution, time - c->startTime->floatValue(), params));
	}

	for (int i = 0; i < resolution; i++)
	{
		float r = 0, g = 0, b = 0;

		for (int j = 0; j < clipColors.size(); j++)
		{
			if (i >= clipColors[j].size()) continue;

			r += clipColors[j][i].getFloatRed();
			g += clipColors[j][i].getFloatGreen();
			b += clipColors[j][i].getFloatBlue();
		}

		result.set(i, (Colour::fromFloatRGBA(jmin(r, 1.f), jmin(g, 1.f), jmin(b, 1.f), 1)));
	}

	return result;
}

SequenceLayerPanel * LightBlockLayer::getPanel()
{
	return new LightBlockLayerPanel(this);
}

SequenceLayerTimeline * LightBlockLayer::getTimelineUI()
{
	return new LightBlockLayerTimeline(this);
}

var LightBlockLayer::getJSONData()
{
	var data = SequenceLayer::getJSONData();
	data.getDynamicObject()->setProperty("blocks", blockClipManager.getJSONData());
	return data;
}

void LightBlockLayer::loadJSONDataInternal(var data)
{
	SequenceLayer::loadJSONDataInternal(data);
	blockClipManager.loadJSONData(data.getProperty("blocks", var()));
}
