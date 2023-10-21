/*
  ==============================================================================

	ParameterLink.cpp
	Created: 21 Dec 2020 11:12:38pm
	Author:  bkupe

  ==============================================================================
*/
#include "Common/CommonIncludes.h"
#include "Prop/PropIncludes.h"
#include "BentoEngine.h"

ParameterLink::ParameterLink(WeakReference<Parameter> p) :
	linkType(NONE),
	parameter(p),
	isLinkable(true),
	//replacementHasMappingInputToken(false),
	paramLinkNotifier(5)
{

}

ParameterLink::~ParameterLink()
{
	//if (list != nullptr && !listRef.wasObjectDeleted())
	//{
	//	list->removeListListener(this);
	//}
}

void ParameterLink::setLinkType(LinkType type)
{
	if (type == linkType) return;
	linkType = type;

	parameter->setControllableFeedbackOnly(linkType != NONE);

	notifyLinkUpdated();
}

void ParameterLink::setLinkedCustomParam(Parameter* p)
{
	if (linkType == CUSTOM_PARAM && linkedCustomParam == p) return;
	linkedCustomParam = p;
	setLinkType(CUSTOM_PARAM);
}

var ParameterLink::getLinkedValue(Prop* prop, int id)
{
	switch (linkType)
	{
	case NONE: return parameter->getValue();
	case OBJECT_ID: return id;
	case CUSTOM_PARAM:
	{
		if (linkedCustomParam == nullptr || linkedCustomParam.wasObjectDeleted()) return parameter->getValue();
		return prop->customParams->getParamValueFor(linkedCustomParam);
	}
	case SOURCE_BLOCK:
		return parameter->getValue();//TODO : implement this
		break;
	}

	return parameter->getValue();
}





WeakReference<Controllable> ParameterLink::getLinkedTarget(Prop* prop)
{
	if (!isLinkable)
	{
		if (parameter->type == Parameter::TARGET) return ((TargetParameter*)parameter.get())->target;
		return nullptr;
	};

	if (linkType == CUSTOM_PARAM)
	{
		if (TargetParameter* p = dynamic_cast<TargetParameter*>(prop->customParams->getActiveParamFor(linkedCustomParam))) return p->target;
	}

	if (parameter->type == Parameter::TARGET) return ((TargetParameter*)parameter.get())->target;
	return nullptr;
}

WeakReference<ControllableContainer> ParameterLink::getLinkedTargetContainer(Prop* prop)
{
	if (!isLinkable)
	{
		if (parameter->type == Parameter::TARGET) return ((TargetParameter*)parameter.get())->targetContainer;
		return nullptr;
	};

	if (linkType == CUSTOM_PARAM)
	{
		if (TargetParameter* p = dynamic_cast<TargetParameter*>(prop->customParams->getActiveParamFor(linkedCustomParam))) return p->targetContainer;
	}

	if (parameter->type == Parameter::TARGET) return ((TargetParameter*)parameter.get())->targetContainer;
	return nullptr;
}

//String ParameterLink::getReplacementString(int multiplexIndex)
//{
//	replacementHasMappingInputToken = false;
//	if (parameter->type != parameter->STRING) return parameter->stringValue();
//
//	std::string s = parameter->stringValue().toStdString();
//	std::regex source_regex("\\{(\\w+:\\w+|index0?)\\}");
//
//	auto source_begin = std::sregex_iterator(s.begin(), s.end(), source_regex);
//	auto source_end = std::sregex_iterator();
//
//	if (std::distance(source_begin, source_end) == 0) return parameter->stringValue();
//
//	int lastPos = 0;
//	String result = "";
//
//	for (std::sregex_iterator i = source_begin; i != source_end; ++i)
//	{
//		std::smatch m = *i;
//		String matchStr(m.str());
//
//		result += parameter->stringValue().substring(lastPos, (int)m.position());
//
//		if (matchStr == "{index}" && isMultiplexed())  result += String(multiplexIndex + 1);
//		else if (matchStr == "{index0}" && isMultiplexed())  result += String(multiplexIndex);
//		else
//		{
//			StringArray dotSplit;
//			dotSplit.addTokens(matchStr.substring(1, matchStr.length() - 1), ":", "");
//			if (dotSplit.size() == 2)
//			{
//				if (dotSplit[0] == "list")
//				{
//					if (isMultiplexed())
//					{
//						if (BaseMultiplexList* curList = multiplex->listManager.getItemWithName(dotSplit[1]))
//						{
//
//							if (Parameter* lp = dynamic_cast<Parameter*>(curList->list[multiplexIndex]))
//							{
//								result += lp->stringValue();
//							}
//							else
//							{
//								result += lp->shortName; // show shortName for triggers, might be useful
//							}
//						}
//					}
//				}
//				else if (dotSplit[0] == "input")
//				{
//					replacementHasMappingInputToken = true;
//					int valueIndex = dotSplit[1].getIntValue() - 1; //1-based to be compliant with UI naming
//					if (mappingValues.size() > 0 && valueIndex >= 0 && valueIndex < mappingValues[multiplexIndex].size()) result += mappingValues[multiplexIndex][valueIndex].toString();
//					else result += "[bad index : " + String(valueIndex) + "]";
//				}
//				else result += matchStr;
//			}
//			else result += matchStr;
//		}
//
//		lastPos = (int)m.position() + matchStr.length();
//	}
//
//	result += parameter->stringValue().substring(lastPos);
//
//	return result;
//}


void ParameterLink::notifyLinkUpdated()
{
	parameterLinkListeners.call(&ParameterLinkListener::linkUpdated, this);
	paramLinkNotifier.addMessage(new ParameterLinkEvent(ParameterLinkEvent::PREVIEW_UPDATED, this));
}


var ParameterLink::getJSONData()
{
	var data(new DynamicObject());
	if (isLinkable)
	{
		data.getDynamicObject()->setProperty("linkType", linkType);
		if (linkType == CUSTOM_PARAM) data.getDynamicObject()->setProperty("linkedCustomParamName", linkedCustomParam->shortName);
	}

	return data;
}

void ParameterLink::loadJSONData(var data)
{
	setLinkType((LinkType)(int)data.getProperty("linkType", NONE));

	if (!data.isObject() || !isLinkable)  return;
	if (linkType == CUSTOM_PARAM)
	{
		if (GenericControllableItem* gci = ((BentoEngine*)Engine::mainEngine)->customParams.getItemWithName(data.getProperty("linkedCustomParamName", "")))
		{
			linkedCustomParam = (Parameter*)gci->controllable;
		}
	}
}

ParamLinkContainer::ParamLinkContainer(const String& name) :
	ControllableContainer(name),
	paramsCanBeLinked(true),
	ghostData(new DynamicObject())
{
}

ParamLinkContainer::~ParamLinkContainer()
{
	paramLinkMap.clear();
	paramLinks.clear();
}

void ParamLinkContainer::onControllableAdded(Controllable* c)
{
	if (!paramsCanBeLinked) return;

	if (Parameter* p = dynamic_cast<Parameter*>(c))
	{
		ParameterLink* pLink = new ParameterLink(p);

		pLink->addParameterLinkListener(this);

		paramLinks.add(pLink);
		paramLinkMap.set(p, pLink);
		linkParamMap.set(pLink, p);

		if (ghostData.hasProperty(pLink->parameter->shortName))
		{
			pLink->loadJSONData(ghostData.getProperty(pLink->parameter->shortName, var()));
		}
	}
}

void ParamLinkContainer::onControllableRemoved(Controllable* c)
{
	if (!paramsCanBeLinked) return;
	if (Parameter* p = dynamic_cast<Parameter*>(c))
	{
		if (paramLinkMap.contains(p))
		{
			if (ParameterLink* pLink = paramLinkMap[p])
			{
				if (ghostData.isVoid()) ghostData = new DynamicObject();
				ghostData.getDynamicObject()->setProperty(pLink->parameter->shortName, pLink->getJSONData());

				pLink->removeParameterLinkListener(this);

				linkParamMap.remove(pLink);
				paramLinkMap.remove(p);
				paramLinks.removeObject(pLink);
			}
		}
	}
}

void ParamLinkContainer::parameterControlModeChanged(Parameter* p)
{
	paramLinkContainerListeners.call(&ParamLinkContainerListener::paramControlModeChanged, this, getLinkedParam(p));
}

ParameterLink* ParamLinkContainer::getLinkedParam(Parameter* p)
{
	if (!paramsCanBeLinked) return nullptr;

	jassert(paramLinkMap.contains(p));
	return paramLinkMap[p];
}

var ParamLinkContainer::getLinkedValue(Parameter* p, Prop* prop, int id, float time)
{
	if (p == nullptr) return var();
	if (!paramsCanBeLinked) return getParamValue(p, time);
	if (ParameterLink* pLink = getLinkedParam(p))
	{
		if (pLink->linkType != ParameterLink::NONE) return pLink->getLinkedValue(prop, id);
	}
	return getParamValue(p, time);
}

var ParamLinkContainer::getParamValue(Parameter* p, float time)
{
	if (p->controlMode != Parameter::AUTOMATION) return p->getValue();

	if (ParameterAutomation* a = p->automation.get())
	{
		if (dynamic_cast<Automation*>(a->automationContainer) != nullptr)
		{
			float value = ((Automation*)a->automationContainer)->getValueAtPosition(fmodf(time, a->lengthParamRef->floatValue()));
			return value;
		}
		else if (dynamic_cast<GradientColorManager*>(a->automationContainer) != nullptr)
		{
			Colour value = ((GradientColorManager*)a->automationContainer)->getColorForPosition(fmodf(time, a->lengthParamRef->floatValue()));
			var result;
			result.append(value.getFloatRed());
			result.append(value.getFloatGreen());
			result.append(value.getFloatBlue());
			result.append(value.getFloatAlpha());
			return result;
		}
	}

	return p->getValue();
}


void ParamLinkContainer::linkUpdated(ParameterLink* p)
{
	paramLinkContainerListeners.call(&ParamLinkContainerListener::linkUpdated, this, p);
}


var ParamLinkContainer::getJSONData()
{
	var data = ControllableContainer::getJSONData();

	var pLinkData(new DynamicObject());
	for (auto& pLink : paramLinks)
	{
		if (pLink->parameter.wasObjectDeleted() || pLink->parameter == nullptr) continue;
		if (pLink->linkType != pLink->NONE) pLinkData.getDynamicObject()->setProperty(pLink->parameter->shortName, pLink->getJSONData());
	}

	data.getDynamicObject()->setProperty("paramLinks", pLinkData);

	return data;
}

void ParamLinkContainer::loadJSONDataInternal(var data)
{
	ghostData = data.getProperty("paramLinks", var()).clone();
	for (auto& pLink : paramLinks)
	{
		if (pLink->parameter == nullptr || pLink->parameter.wasObjectDeleted()) continue;

		if (ghostData.isObject() && ghostData.hasProperty(pLink->parameter->shortName))
		{
			pLink->loadJSONData(ghostData.getProperty(pLink->parameter->shortName, var()));
			ghostData.getDynamicObject()->removeProperty(pLink->parameter->shortName);
		}
		else
		{
			pLink->setLinkType(ParameterLink::NONE);
		}
	}

}

InspectableEditor* ParamLinkContainer::getEditorInternal(bool isRoot, Array<Inspectable*> inspectables)
{
	return new ParamLinkContainerEditor(this, isRoot);
}