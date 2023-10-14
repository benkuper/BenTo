/*
  ==============================================================================

    CompositeNode.cpp
    Created: 13 Apr 2018 11:30:02pm
    Author:  Ben

  ==============================================================================
*/

CompositeNode::CompositeNode(var params) :
	ColorNode(getTypeString(), params),
	mix(nullptr)
{
	c1 = addColorSlot(true, "Colors 1");
	c2 = addColorSlot(true, "Colors 2");

	blendMode = addEnumParameter("Blend Mode", "Blend Mode");
	blendMode->addOption("Add", ADD)->addOption("Alpha",ALPHA)->addOption("Subtract", SUBTRACT)->addOption("Mix", MIX)->addOption("Max", MAX)->addOption("Min", MIN);
}

CompositeNode::~CompositeNode()
{
}

Array<Colour> CompositeNode::getColorsInternal(Prop * p, double time, var params, var localParams)
{
	Array<Colour> col1 = c1->getColors(p, time, params);
	Array<Colour> col2 = c2->getColors(p, time, params);
	
	BlendMode b = (BlendMode)(int)getParameterValue(blendMode, localParams);

	Array<Colour> result;
	int resolution = p->getResolution();
	for (int i = 0; i < resolution; i++)
	{

		switch (b)
		{
		case ADD:
			result.add(Colour::fromRGBA(
				jmin<int>(col1[i].getRed()* col1[i].getFloatAlpha() + col2[i].getRed()* col2[i].getFloatAlpha(), 255),
				jmin<int>(col1[i].getGreen()* col1[i].getFloatAlpha() + col2[i].getGreen()* col2[i].getFloatAlpha(), 255),
				jmin<int>(col1[i].getBlue()* col1[i].getFloatAlpha() + col2[i].getBlue()* col2[i].getFloatAlpha(), 255),
				jmax<int>(col1[i].getAlpha(), col2[i].getAlpha()
				)
			));
			break;

		case ALPHA:
			result.add(col1[i].interpolatedWith(col2[i], col2[i].getFloatAlpha()));
			break;


		case SUBTRACT:
			result.add(Colour::fromRGBA(
				jmin(col2[i].getRed() - col1[i].getRed(), 0),
				jmin(col2[i].getGreen() - col1[i].getGreen(), 0),
				jmin(col2[i].getBlue() - col1[i].getBlue(), 0),
				jmin(col2[i].getAlpha() - col1[i].getAlpha(), 0)
			));
			break;

		case MIX:
			if(mix != nullptr) result.add(col1[i].interpolatedWith(col2[i], mix->floatValue()));
			break;

		case MAX:
			result.add(Colour::fromRGBA(
				jmax(col2[i].getRed(),col1[i].getRed()),
				jmax(col2[i].getGreen(),col1[i].getGreen()),
				jmax(col2[i].getBlue(),col1[i].getBlue()),
				jmax(col2[i].getAlpha(), col1[i].getAlpha())
				));
			break;

		case MIN:
			result.add(Colour::fromRGBA(
				jmin(col2[i].getRed(), col1[i].getRed()),
				jmin(col2[i].getGreen(), col1[i].getGreen()),
				jmin(col2[i].getBlue(), col1[i].getBlue()),
				jmin(col2[i].getAlpha(), col1[i].getAlpha())
			));
			break;
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
