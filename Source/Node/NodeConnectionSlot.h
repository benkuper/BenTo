/*
==============================================================================

NodeConnectionSlot.h
Created: 14 Apr 2018 5:45:15pm
Author:  Ben

==============================================================================
*/

#pragma once

#include "JuceHeader.h"
class Prop;
class Node;
class NodeConnection;

enum ConnectionType { Color, ColorBlock, Number };

class NodeConnectionSlot
{
public:
	NodeConnectionSlot(Node * node, bool isInput, StringRef name, StringRef id, ConnectionType type) : node(node), isInput(isInput), name(name), id(id), type(type) {}
	virtual ~NodeConnectionSlot() { masterReference.clear(); }

	Node * node;
	bool isInput;
	String name;
	String id;
	ConnectionType type;

	Array<NodeConnection *> connections;
	bool isConnected();
	
	virtual void addConnection(NodeConnection * c);
	virtual void removeConnection(NodeConnection *c);


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
