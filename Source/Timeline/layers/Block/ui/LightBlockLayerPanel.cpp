/*
  ==============================================================================

    LightBlockLayerPanel.cpp
    Created: 17 Apr 2018 7:15:39pm
    Author:  Ben

  ==============================================================================
*/

LightBlockLayerPanel::LightBlockLayerPanel(LightBlockLayer * layer) :
	SequenceLayerPanel(layer),
	blockLayer(layer)
{
	previewIDUI.reset(layer->previewID->createStepper());
	previewIDUI->showLabel = true;
	contentComponents.add(previewIDUI.get());

	addAndMakeVisible(previewIDUI.get());
	minContentHeight = 80;
}

LightBlockLayerPanel::~LightBlockLayerPanel()
{
}

void LightBlockLayerPanel::resizedInternalContent(Rectangle<int>& r)
{
	previewIDUI->setBounds(r.removeFromTop(16));
}
