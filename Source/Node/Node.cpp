/*
  ==============================================================================

	Node.cpp
	Created: 14 Apr 2018 2:11:31am
	Author:  Ben

  ==============================================================================
*/

Node::Node(const String & name, var) :
	BaseItem(name),
	nodeNotifier(10)
{
	itemDataType = "Node";
}

Node::~Node()
{
	masterReference.clear();
}

ParameterSlot * Node::addParameterSlot(bool isInput, Parameter * p)
{
	if (p->type != Parameter::FLOAT && p->type != Parameter::INT && p->type != Parameter::COLOR)
	{
		DBG("Other types are not supported !");
	}

	ParameterSlot * s = static_cast<ParameterSlot *>(getSlotWithName(isInput, p->niceName));
	if (s == nullptr)
	{
		s = new ParameterSlot(this, isInput, p);
		s->addSlotListener(this);
		if (isInput) inSlots.add(s);
		else outSlots.add(s);
		nodeListeners.call(&NodeListener::slotAdded, this, s, isInput);
		nodeNotifier.addMessage(new NodeEvent(isInput ? NodeEvent::INSLOT_ADDED : NodeEvent::OUTSLOT_ADDED, this, s, s->name));
	}

	return s;
}

ColorSlot * Node::addColorSlot(bool isInput, const String & name)
{
	ColorSlot * s = static_cast<ColorSlot *>(getSlotWithName(isInput, name));
	if (s == nullptr)
	{
		s = new ColorSlot(this, isInput, name);
		s->addSlotListener(this);
		if (isInput) inSlots.add(s);
		else outSlots.add(s);
		nodeListeners.call(&NodeListener::slotAdded, this, s, isInput);
		nodeNotifier.addMessage(new NodeEvent(isInput ? NodeEvent::INSLOT_ADDED : NodeEvent::OUTSLOT_ADDED, this, s, s->name));
	}
	return s;
}

void Node::removeSlot(bool isInput, const String & name)
{
	NodeConnectionSlot * s = getSlotWithName(isInput, name);
	removeSlot(isInput, s);
}

void Node::removeSlot(bool isInput, NodeConnectionSlot * s)
{
	if (s != nullptr)
	{
		s->removeSlotListener(this);
		if (isInput)
		{
			inSlots.getLock().enter();
			inSlots.removeObject(s, false);
			inSlots.getLock().exit();
		}
		else outSlots.removeObject(s, false);
		nodeListeners.call(&NodeListener::slotRemoved, this, s, isInput);
		nodeNotifier.addMessage(new NodeEvent(isInput ? NodeEvent::INSLOT_REMOVED : NodeEvent::OUTSLOT_REMOVED, this, s, s->name));
		delete s;
	}
}

NodeConnectionSlot * Node::getSlotWithName(bool isInput, const String & name)
{
	if (isInput) {
		for (auto & s : inSlots) if (s->name == name) return s;
	}
	else
	{
		for (auto & s : outSlots) if (s->name == name) return s;
	}

	return nullptr;
}

Array<NodeConnectionSlot*> Node::getSlotsWithType(bool isInput, ConnectionType type)
{
	Array<NodeConnectionSlot*> result;
	if (isInput)
	{
		for (auto & s : inSlots) if (s->type == type) result.add(s);
	}
	else
	{
		for (auto & s : outSlots) if (s->type == type) result.add(s);
	}

	return result;
}

ParameterSlot * Node::getParameterSlot(Parameter * p, bool isInput)
{
	if (isInput)
	{
		for (auto & s : inSlots)
		{
			if (s->type == ConnectionType::ColorBlock) continue;
			ParameterSlot * ps = dynamic_cast<ParameterSlot *>(s);
			if (ps == nullptr) continue;
			if (ps->parameter == p) return ps;
		}
	}
	else
	{
		for (auto & s : outSlots)
		{
			if (s->type == ConnectionType::ColorBlock) continue;
			ParameterSlot * ps = dynamic_cast<ParameterSlot *>(s);
			if (ps == nullptr) continue;
			if (ps->parameter == p) return ps;
		}
	}

	return nullptr;
}

var Node::getParameterValue(Parameter * p, var params)
{
	NodeConnectionSlot * slot = getParameterSlot(p, true);
	if (slot == nullptr || !slot->isConnected()) return p->getValue();

	Node * n = slot->connections[0]->sourceSlot->node;
	if (n == nullptr) return p->getValue();

	return params.getProperty(n->shortName, p->getValue());
}

void Node::fillWithLocalParams(var params)
{
	inSlots.getLock().enter();
	for (auto &s : inSlots)
	{
		if (s->type == ConnectionType::ColorBlock) continue;

		if (s->isConnected())
		{
			if (s->connections[0]->sourceSlot == nullptr) continue;
			ParameterNode * pn = dynamic_cast<ParameterNode *>(s->connections[0]->sourceSlot->node);
			if (pn == nullptr) continue;
			if (params.isVoid()) params = new DynamicObject();

			NamedValueSet set = params.getDynamicObject()->getProperties();
			
			if (params.hasProperty(pn->shortName))
			{
				var value = params.getProperty(pn->shortName, pn->parameter->getValue());
				params.getDynamicObject()->setProperty(s->id, value);
			}
		}
		
	}
	inSlots.getLock().exit();
}

NodeViewUI * Node::createUI()
{
	return new NodeViewUI(this);
}