/*
  ==============================================================================

    ParameterNode.h
    Created: 13 Apr 2018 11:30:12pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

class ParameterNode :
	public Node
{
public:
	ParameterNode(var params = var());
	~ParameterNode();

	String paramType;
	Parameter * parameter;

	virtual void connectionAdded(NodeConnectionSlot * s, NodeConnection * c) override;

	String getTypeString() const override { return paramType; }
	static ParameterNode * create(var params) { return new ParameterNode(params); }

	virtual NodeViewUI * createUI() override;

};