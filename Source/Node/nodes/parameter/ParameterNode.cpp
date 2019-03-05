/*
  ==============================================================================

    ParameterNode.cpp
    Created: 13 Apr 2018 11:30:12pm
    Author:  Ben

  ==============================================================================
*/

#include "ParameterNode.h"
#include "ParameterNodeViewUI.h"

ParameterNode::ParameterNode(var params) :
	Node(params.getProperty("type", "Parameter"), params),
	paramType(params.getProperty("type", "Parameter").toString())
{ 
	parameter = static_cast<Parameter *>(ControllableFactory::createControllable(paramType));
	jassert(parameter != nullptr);

	parameter->isCustomizableByUser = true;

	parameter->setNiceName("Value");
	parameter->saveValueOnly = false;
	addParameter(parameter);
	addParameterSlot(false, parameter);
}

ParameterNode::~ParameterNode()
{
}


NodeViewUI * ParameterNode::createUI()
{
	return new ParameterNodeViewUI(this);
}
