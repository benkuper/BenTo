/*
  ==============================================================================

    ColorRemapNode.cpp
    Created: 13 Apr 2018 11:29:57pm
    Author:  Ben

  ==============================================================================
*/

ColorRemapNode::ColorRemapNode(var params) :
	ColorNode(getTypeString(), params),
    colorManager(1)
{
    saveAndLoadRecursiveData = true;

    c1 = addColorSlot(true, "Colors 1");

    inputModeParam = addEnumParameter("Input Mode", "Input channel to map the colors to");
    inputModeParam->addOption("Brightness", BRIGHTNESS)->addOption("Hue", HUE)->addOption("Saturation", SATURATION);

    colorManager.allowKeysOutside = false;
    colorManager.addColorAt(0, RED_COLOR);
    colorManager.addColorAt(.5f, YELLOW_COLOR);
    colorManager.addColorAt(1, GREEN_COLOR);
	addChildControllableContainer(&colorManager);
}

ColorRemapNode::~ColorRemapNode()
{
}

Array<Colour> ColorRemapNode::getColorsInternal(Prop* p, double time, var params)
{

    Array<Colour> col1 = c1->getColors(p, time, params);
    InputMode im = (InputMode)(int)getParameterValue(inputModeParam, params);
    
	Array<Colour> result;
	int resolution = p->resolution->intValue();

	for (int i = 0; i < resolution; i++)
	{
        float val = im == BRIGHTNESS ? col1[i].getBrightness() : (im == HUE ? col1[i].getHue() : col1[i].getSaturation());
        result.add(colorManager.getColorForPosition(val));
	}

    return result;
}