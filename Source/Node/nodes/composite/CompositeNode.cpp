/*
  ==============================================================================

    CompositeNode.cpp
    Created: 13 Apr 2018 11:30:02pm
    Author:  Ben

  ==============================================================================
*/

#include "CompositeNode.h"


CompositeNode::CompositeNode(var params) :
	ColorNode(getTypeString(), params)
{
	c1 = addColorSlot(true, "Colors 1");
	c2 = addColorSlot(true, "Colors 2");

	blendMode = addEnumParameter("Blend Mode", "Blend Mode");
	blendMode->addOption("Add", ADD)->addOption("Subtract", SUBTRACT)->addOption("Mix", MIX)->addOption("Max", MAX)->addOption("Min", MIN);
}

CompositeNode::~CompositeNode()
{
}

Array<Colour> CompositeNode::getColorsForProp(Prop * p)
{
	Array<Colour> col1 = c1->getColorsForProp(p);
	Array<Colour> col2 = c2->getColorsForProp(p);
	
	BlendMode b = blendMode->getValueDataAsEnum<BlendMode>();

	Array<Colour> result;
	for (int i = 0; i < p->resolution->intValue(); i++)
	{
		switch (b)
		{
		case ADD:
			result.add(Colour::fromRGB(
				jmin(col1[i].getRed() + col2[i].getRed(), 255),
				jmin(col1[i].getGreen() + col2[i].getGreen(), 255),
				jmin(col1[i].getBlue() + col2[i].getBlue(), 255)));
			break;
		case SUBTRACT:
			result.add(Colour::fromRGB(
				jmin(col2[i].getRed() - col1[i].getRed(), 0),
				jmin(col2[i].getGreen() - col1[i].getGreen(), 0),
				jmin(col2[i].getBlue() - col1[i].getBlue(), 0)));
			break;
		case MIX:
			result.add(col1[i].interpolatedWith(col2[i], .5f));
			break;

		case MAX:
			result.add(Colour::fromRGB(
				jmax(col2[i].getRed(),col1[i].getRed()),
				jmax(col2[i].getGreen(),col1[i].getGreen()),
				jmax(col2[i].getBlue(),col1[i].getBlue())));
			break;

		case MIN:
			result.add(Colour::fromRGB(
				jmin(col2[i].getRed(), col1[i].getRed()),
				jmin(col2[i].getGreen(), col1[i].getGreen()),
				jmin(col2[i].getBlue(), col1[i].getBlue())));
			break;
		}
		
	}
	return result;
}
