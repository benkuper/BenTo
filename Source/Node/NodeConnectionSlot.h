/*
==============================================================================

NodeConnectionSlot.h
Created: 14 Apr 2018 5:45:15pm
Author:  Ben

==============================================================================
*/

#pragma once

class Prop;
class Node;
class NodeConnection;

enum ConnectionType { Color, ColorBlock, Number };

class NodeConnectionSlot
{
public:
	NodeConnectionSlot(Node * node, bool isInput, StringRef name, StringRef id, ConnectionType type);
	virtual ~NodeConnectionSlot();

	Node * node;
	bool isInput;
	String name;
	String id;
	ConnectionType type;

	Array<NodeConnection *> connections;
	bool isConnected();
	
	virtual void addConnection(NodeConnection * c);
	virtual void removeConnection(NodeConnection *c);

	class  SlotListener
	{
	public:
		/** Destructor. */
		virtual ~SlotListener() {}
		virtual void connectionAdded(NodeConnectionSlot *, NodeConnection *) {}
		virtual void connectionRemoved(NodeConnectionSlot *, NodeConnection *) {}
	};


	ListenerList<SlotListener> slotListeners;
	void addSlotListener(SlotListener * newListener) { slotListeners.add(newListener); }
	void removeSlotListener(SlotListener * listener) { slotListeners.remove(listener); }

	WeakReference<NodeConnectionSlot>::Master masterReference;
};

class ColorSlot :
	public NodeConnectionSlot
{
public:
	ColorSlot(Node * node, bool isInput, const String &name) : NodeConnectionSlot(node, isInput, name, name, ConnectionType::ColorBlock) {}

	Array<Colour> getColors(Prop * p, double time, var params);
};

class ParameterSlot :
	public NodeConnectionSlot,
	public Parameter::ParameterListener
{
public:
	ParameterSlot(Node * node, bool isInput, Parameter * p) :
		NodeConnectionSlot(node, isInput, p->niceName, p->shortName, p->type == Parameter::COLOR ? ConnectionType::Color : ConnectionType::Number),
		parameter(p)
	{
		parameter->setValue(getValue());
		if(!isInput) parameter->addParameterListener(this);
	}

	void handleParameterUpdate(Parameter * p);
	void addConnection(NodeConnection * c) override;
	void removeConnection(NodeConnection *c) override;
	void parameterValueChanged(Parameter * p) override;

	var getValue();
	Parameter * parameter;
};
