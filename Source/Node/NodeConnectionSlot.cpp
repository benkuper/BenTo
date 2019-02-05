/*
  ==============================================================================

    NodeConnectionSlot.cpp
    Created: 14 Apr 2018 5:45:15pm
    Author:  Ben

  ==============================================================================
*/

#include "NodeConnectionSlot.h"
#include "NodeConnection.h"
#include "nodes/parameter/ParameterNode.h"
#include "ColorNode.h"


var ParameterSlot::getValue()
{
	if (isInput)
	{
		if (connections.size() > 0)
		{
			ParameterSlot * s = static_cast<ParameterSlot *>(connections[0]->sourceSlot.get());
			if (s != nullptr) return s->parameter->value;
		}
	}	

	return parameter->value;
}

Array<Colour> ColorSlot::getColors(Prop * p, double time, var params)
{
	if (isInput)
	{
		if (connections.size() > 0)
		{
			ColorSlot * s = static_cast<ColorSlot *>(connections[0]->sourceSlot.get());
			if (s != nullptr) return s->getColors(p, time, params);
		}
	} else
	{
		return static_cast<ColorNode *>(node)->getColors(p, time, params);
	}
	
	Array<Colour> result;
	result.resize(p->resolution->intValue());
	result.fill(Colours::black);

	return result;
}

bool NodeConnectionSlot::isConnected()
{
	return connections.size() > 0;
}

void NodeConnectionSlot::addConnection(NodeConnection * c)
{
	connections.add(c);
}

void NodeConnectionSlot::removeConnection(NodeConnection * c)
{
	connections.removeAllInstancesOf(c);
}


void ParameterSlot::handleParameterUpdate(Parameter * p)
{
	parameter->setValue(p->value);
}

void ParameterSlot::addConnection(NodeConnection * nc)
{
	NodeConnectionSlot::addConnection(nc);
	if(isInput) parameter->setControllableFeedbackOnly(true);
}

void ParameterSlot::removeConnection(NodeConnection * nc)
{
	NodeConnectionSlot::removeConnection(nc);
	if (isInput && connections.size() == 0) parameter->setControllableFeedbackOnly(false);
}

void ParameterSlot::parameterValueChanged(Parameter * p)
{
	if (!isInput)
	{
		for (auto & c : connections) c->dispatchParameterUpdate(parameter);
	}
}
