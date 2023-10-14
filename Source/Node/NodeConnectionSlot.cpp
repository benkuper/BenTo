/*
  ==============================================================================

    NodeConnectionSlot.cpp
    Created: 14 Apr 2018 5:45:15pm
    Author:  Ben

  ==============================================================================
*/

var ParameterSlot::getValue()
{
	if (isInput)
	{
		if (connections.size() > 0)
		{
			ParameterSlot * s = static_cast<ParameterSlot *>(connections[0]->sourceSlot.get());
			if (s != nullptr) return s->parameter->getValue();
		}
	}	

	return parameter->getValue();
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
	result.resize(p->getResolution());
	result.fill(Colours::transparentBlack);

	return result;
}

NodeConnectionSlot::NodeConnectionSlot(Node * node, bool isInput, StringRef name, StringRef id, ConnectionType type) : 
	node(node),
	isInput(isInput),
	name(name), 
	id(id), 
	type(type) 
{

}

NodeConnectionSlot::~NodeConnectionSlot() 
{ 
	while (connections.size() > 0) connections[0]->remove();
	masterReference.clear(); 
}

bool NodeConnectionSlot::isConnected()
{
	return connections.size() > 0;
}

void NodeConnectionSlot::addConnection(NodeConnection * c)
{
	connections.add(c);
	slotListeners.call(&SlotListener::connectionAdded, this, c);
}

void NodeConnectionSlot::removeConnection(NodeConnection * c)
{
	connections.removeAllInstancesOf(c);
	slotListeners.call(&SlotListener::connectionRemoved, this, c);
}


void ParameterSlot::handleParameterUpdate(Parameter * p)
{
	parameter->setValue(p->getValue());
}

void ParameterSlot::addConnection(NodeConnection * nc)
{
	NodeConnectionSlot::addConnection(nc);
	if(isInput) parameter->setControllableFeedbackOnly(true);}

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
