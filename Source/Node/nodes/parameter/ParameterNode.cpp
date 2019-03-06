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

void ParameterNode::connectionAdded(NodeConnectionSlot * s, NodeConnection * c)
{
	if (!parameter->canHaveRange) return;
	
	ParameterSlot * ps = dynamic_cast<ParameterSlot *>(c->destSlot.get());
	if (ps != nullptr && s->connections.size() == 1) parameter->setRange(ps->parameter->minimumValue, ps->parameter->maximumValue);
}


NodeViewUI * ParameterNode::createUI()
{
	return new ParameterNodeViewUI(this);
}
