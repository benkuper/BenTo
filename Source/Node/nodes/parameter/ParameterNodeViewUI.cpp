/*
  ==============================================================================

    ParameterNodeViewUI.cpp
    Created: 14 Apr 2018 5:07:05pm
    Author:  Ben

  ==============================================================================
*/

#include "ParameterNodeViewUI.h"

ParameterNodeViewUI::ParameterNodeViewUI(ParameterNode * pn) :
	NodeViewUI(pn),
	pn(pn)
{
	pui = pn->parameter->createDefaultUI();
	addAndMakeVisible(pui); 
}

ParameterNodeViewUI::~ParameterNodeViewUI()
{
}

void ParameterNodeViewUI::resizedInternalContent(Rectangle<int>& r)
{
	pui->setBounds(r.reduced(4));
}