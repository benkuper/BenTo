/*
  ==============================================================================

    ParameterNode.cpp
    Created: 13 Apr 2018 11:30:12pm
    Author:  Ben

  ==============================================================================
*/

#include "ParameterNode.h"


ParameterNode::ParameterNode(var params) :
	Node(getTypeString(), params)
{
	String t = params.getProperty("type", "");
	parameter = static_cast<Parameter *>(ControllableFactory::createControllable(t));
	jassert(parameter != nullptr);

	parameter->setNiceName("Out value");
	addParameter(parameter);
	addParameterSlot(false, parameter);
}

ParameterNode::~ParameterNode()
{
}
