/*
  ==============================================================================

    LightBlockLayerPanel.cpp
    Created: 17 Apr 2018 7:15:39pm
    Author:  Ben

  ==============================================================================
*/

#include "LightBlockLayerPanel.h"

LightBlockLayerPanel::LightBlockLayerPanel(LightBlockLayer * layer) :
	SequenceLayerPanel(layer),
	blockLayer(layer)
{
}

LightBlockLayerPanel::~LightBlockLayerPanel()
  {
  }
