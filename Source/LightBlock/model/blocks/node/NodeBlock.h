/*
  ==============================================================================

    NodeBlock.h
    Created: 10 Apr 2018 6:59:08pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

class ParameterNode;

class NodeBlock :
	public LightBlockModel,
	public NodeManager::ManagerListener
{
public:
	NodeBlock(var params = var());
	~NodeBlock();

	NodeManager manager;

	HashMap<Parameter *, ParameterNode *> paramToNodeMap;
	HashMap<ParameterNode *, Parameter *> nodeToParamMap;

	Array<Colour> getColors(Prop * p, double time, var params) override;

	void itemAdded(Node*) override;
	void itemsAdded(Array<Node *>) override;
	void itemRemoved(Node*) override;
	void itemsRemoved(Array<Node*>) override;

	void childAddressChanged(ControllableContainer * cc) override;
	
	void onExternalParameterValueChanged(Parameter * p) override;
	void onExternalParameterRangeChanged(Parameter * p) override;

	var getJSONData() override;
	void loadJSONDataInternal(var data) override;

	DECLARE_TYPE("Node")


	LightBlockModelUI * createUI() override;
};
