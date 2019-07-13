/*
  ==============================================================================

	NodeBlock.cpp
	Created: 10 Apr 2018 6:59:08pm
	Author:  Ben

  ==============================================================================
*/

#include "NodeBlock.h"
#include "Node/nodes/prop/PropNode.h"
#include "ui/NodeBlockUI.h"
#include "Node/nodes/parameter/ParameterNode.h"

NodeBlock::NodeBlock(var params) :
	LightBlockModel(getTypeString(), params)
{
	addChildControllableContainer(&manager);
	manager.addBaseManagerListener(this);
	manager.hideInEditor = true;
}

NodeBlock::~NodeBlock()
{
	manager.removeBaseManagerListener(this); 
}

Array<Colour> NodeBlock::getColors(Prop * p, double time, var params)
{
	return  manager.propNode->getColors(p, time, params);
}


void NodeBlock::itemAdded(Node * n)
{
	ParameterNode * pn = dynamic_cast<ParameterNode *>(n);
	if (pn == nullptr) return;
	if (paramsContainer->getParameterByName(pn->shortName) != nullptr) return;
	
	Parameter * p = ControllableFactory::createParameterFrom(pn->parameter, false, true);
	p->setNiceName(pn->niceName);
	paramToNodeMap.set(p, pn);
	nodeToParamMap.set(pn, p);

	pn->addControllableContainerListener(this);

	pn->parameter->addParameterListener(this);
	p->addParameterListener(this);
	paramsContainer->addParameter(p);
	providerListeners.call(&ProviderListener::providerParametersChanged, this);
}

void NodeBlock::itemRemoved(Node * n)
{
	ParameterNode * pn = dynamic_cast<ParameterNode *>(n);
	if (pn == nullptr) return;
	WeakReference<Parameter> p = paramsContainer->getParameterByName(pn->shortName);
	
	if (p == nullptr || p.wasObjectDeleted()) return;

	if(paramToNodeMap.contains(p)) nodeToParamMap.remove(paramToNodeMap[p]);
	paramToNodeMap.remove(p);

	pn->parameter->removeParameterListener(this);
	p->removeParameterListener(this);
	paramsContainer->removeControllable(p.get());
	pn->removeControllableContainerListener(this);


	providerListeners.call(&ProviderListener::providerParametersChanged, this);
}

void NodeBlock::childAddressChanged(ControllableContainer * cc)
{
	ParameterNode * pn = dynamic_cast<ParameterNode *>(cc);
	if (pn != nullptr)
	{
		//DBG("Child address changed : " << cc->niceName);
		Parameter * p = nodeToParamMap[pn];
		if (p != nullptr) p->setNiceName(cc->niceName);
	}
	else
	{
		LightBlockModel::childAddressChanged(cc);
	}
}


void NodeBlock::onExternalParameterValueChanged(Parameter * p)
{
	if (p->parentContainer == paramsContainer.get())
	{
		paramToNodeMap[p]->parameter->setValue(p->getValue());
	}
	else
	{
		ParameterNode * pn = p->getParentAs<ParameterNode>();
		if (pn != nullptr) nodeToParamMap[pn]->setValue(pn->parameter->getValue());
	}
}

void NodeBlock::onExternalParameterRangeChanged(Parameter * p)
{
	if (p->parentContainer == paramsContainer.get())
	{
		paramToNodeMap[p]->parameter->setRange(p->minimumValue, p->maximumValue);
	}
	else
	{
		ParameterNode * pn = p->getParentAs<ParameterNode>();
		if (pn != nullptr) nodeToParamMap[pn]->setRange(pn->parameter->minimumValue, pn->parameter->maximumValue);
	}
}

var NodeBlock::getJSONData()
{
	var data = LightBlockModel::getJSONData();
	data.getDynamicObject()->setProperty("nodes", manager.getJSONData());
	return data;
}

void NodeBlock::loadJSONDataInternal(var data)
{
	LightBlockModel::loadJSONDataInternal(data);
	manager.loadJSONDataInternal(data.getProperty("nodes", ""));
}

LightBlockModelUI * NodeBlock::createUI()
{
	return new NodeBlockUI(this);
}
