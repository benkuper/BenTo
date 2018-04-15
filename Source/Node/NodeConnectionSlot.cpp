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

Array<Colour> ColorSlot::getColorsForProp(Prop * p)
{
	if (isInput)
	{
		if (connections.size() > 0)
		{
			ColorSlot * s = static_cast<ColorSlot *>(connections[0]->sourceSlot.get());
			if (s != nullptr) return s->getColorsForProp(p);
		}
	} else
	{
		return static_cast<ColorNode *>(node)->getColorsForProp(p);
	}
	
	Array<Colour> result;
	for (int i = 0; i < p->resolution->intValue(); i++)
	{
		result.add(Colours::black);
	}
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
