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
}

Array<Colour> NodeBlock::getColors(Prop * p, double time, var params)
{
	return  manager.propNode->getColors(p, time, params);
}

Array<WeakReference<Controllable>> NodeBlock::getModelParameters()
{
	return manager.getExposedParameters();
}

void NodeBlock::itemAdded(Node * n)
{
	ParameterNode * pn = dynamic_cast<ParameterNode *>(n);
	if (pn == nullptr) return;
	if (paramsContainer->getParameterByName(pn->shortName) != nullptr) return;
	Parameter * p = ControllableFactory::createParameterFrom(pn->parameter, false, true);
	p->setNiceName(pn->niceName);
	paramsContainer->addParameter(p);
	providerListeners.call(&ProviderListener::providerParametersChanged, this);
}

void NodeBlock::itemRemoved(Node * n)
{
	ParameterNode * pn = dynamic_cast<ParameterNode *>(n);
	if (pn == nullptr) return;
	Parameter * p = paramsContainer->getParameterByName(pn->shortName);
	if (p == nullptr) return;
	paramsContainer->removeControllable(p);
	providerListeners.call(&ProviderListener::providerParametersChanged, this);
}

void NodeBlock::childAddressChanged(ControllableContainer * cc)
{
	LightBlockModel::childAddressChanged(cc);
	DBG("Child address changed : " << cc->niceName);
}

void NodeBlock::childStructureChanged(ControllableContainer * cc)
{
	LightBlockModel::childStructureChanged(cc);
	DBG("Child structure changed : " << cc->niceName);
}

void NodeBlock::updateParametersFromNode()
{

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
