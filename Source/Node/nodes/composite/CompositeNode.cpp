/*
  ==============================================================================

	CompositeNode.cpp
	Created: 13 Apr 2018 11:30:02pm
	Author:  Ben

  ==============================================================================
*/

#include "Node/NodeIncludes.h"
#include "Prop/PropIncludes.h"


CompositeNode::CompositeNode(var params) :
	ColorNode(getTypeString(), params),
	mix(nullptr)
{
	c1 = addColorSlot(true, "Colors 1");
	c2 = addColorSlot(true, "Colors 2");
	c3 = addColorSlot(true, "Colors 3");
	c4 = addColorSlot(true, "Colors 4");

	blendMode = addEnumParameter("Blend Mode", "Blend Mode");
	blendMode->addOption("Add", ADD)->addOption("Alpha", ALPHA)->addOption("Subtract", SUBTRACT)->addOption("Mix", MIX)->addOption("Max", MAX)->addOption("Min", MIN);
}

CompositeNode::~CompositeNode()
{
}

Array<Colour> CompositeNode::getColorsInternal(Prop* p, double time, var params, var localParams)
{
	Array<Array<Colour>> colors;
	if (c1->isConnected()) colors.add(c1->getColors(p, time, params));
	if (c2->isConnected()) colors.add(c2->getColors(p, time, params));
	if (c3->isConnected()) colors.add(c3->getColors(p, time, params));
	if (c4->isConnected()) colors.add(c4->getColors(p, time, params));

	BlendMode b = (BlendMode)(int)getParameterValue(blendMode, localParams);

	if (colors.size() == 0) return Array<Colour>();

	int resolution = p->getResolution();

	Array<Colour> result;
	result.addArray(colors[0].getRawDataPointer(), resolution);

	for (int c = 1; c < colors.size(); c++)
	{
		for (int i = 0; i < resolution; i++)
		{
			Colour oc = result[i];
			Colour nc = colors[c][i];

			switch (b)
			{
			case ADD:
				result.set(i, Colour::fromRGBA(
					jmin<int>(oc.getRed() * oc.getFloatAlpha() + nc.getRed() * nc.getFloatAlpha(), 255),
					jmin<int>(oc.getGreen() * oc.getFloatAlpha() + nc.getGreen() * nc.getFloatAlpha(), 255),
					jmin<int>(oc.getBlue() * oc.getFloatAlpha() + nc.getBlue() * nc.getFloatAlpha(), 255),
					jmax<int>(oc.getAlpha(), nc.getAlpha()))
				);
				break;


			case ALPHA:
				result.set(i, oc.interpolatedWith(nc, nc.getFloatAlpha()));
				break;


			case SUBTRACT:
				result.set(i, Colour::fromRGBA(
					jmin(nc.getRed() - oc.getRed(), 0),
					jmin(nc.getGreen() - oc.getGreen(), 0),
					jmin(nc.getBlue() - oc.getBlue(), 0),
					jmin(nc.getAlpha() - oc.getAlpha(), 0)
				));
				break;

			case MIX:
				if (mix != nullptr) result.add(oc.interpolatedWith(nc, mix->floatValue()));
				break;

			case MAX:
				result.set(i, Colour::fromRGBA(
					jmax(nc.getRed(), oc.getRed()),
					jmax(nc.getGreen(), oc.getGreen()),
					jmax(nc.getBlue(), oc.getBlue()),
					jmax(nc.getAlpha(), oc.getAlpha())
				));
				break;

			case MIN:
				result.set(i, Colour::fromRGBA(
					jmin(nc.getRed(), oc.getRed()),
					jmin(nc.getGreen(), oc.getGreen()),
					jmin(nc.getBlue(), oc.getBlue()),
					jmin(nc.getAlpha(), oc.getAlpha())
				));
				break;
			}

		}
	}

	return result;
}

void CompositeNode::onContainerParameterChangedInternal(Parameter* p)
{
	ColorNode::onContainerParameterChangedInternal(p);

	if (p == blendMode)
	{
		if (blendMode->getValueDataAsEnum<BlendMode>() == MIX)
		{
			mix = addFloatParameter("Mix", "Mix weight between color 1 and color 2", .5f, 0, 1);
		}
		else
		{
			if (mix != nullptr)
			{
				removeControllable(mix);
				mix = nullptr;
			}
		}
	}
}
