/*
  ==============================================================================

	PropFamily.cpp
	Created: 5 Feb 2019 11:25:04am
	Author:  jonglissimo

  ==============================================================================
*/

//PropFamily::PropFamily(var params) :
//	ControllableContainer(params.getProperty("name", "Mistery Family"))
//{
//	createControllablesForContainer(params.getProperty("parameters",var()), this);
//}
//
//PropFamily::~PropFamily()
//{
//}
//
//void PropFamily::createControllablesForContainer(var data, ControllableContainer* cc)
//{
//	if (data.isVoid()) return;
//
//	NamedValueSet valueProps = data.getDynamicObject()->getProperties();
//	for (auto& p : valueProps)
//	{
//		if (!p.value.isObject()) continue;
//
//		if (p.value.getProperty("type", "") == "Container")
//		{
//			ControllableContainer* childCC = cc->getControllableContainerByName(p.name.toString(), true);
//
//			if (childCC == nullptr)
//			{
//				int index = p.value.getProperty("index", -1);
//				if (p.value.getProperty("canBeDisabled", false))
//				{
//					childCC = new EnablingControllableContainer(p.name.toString());
//					((EnablingControllableContainer*)childCC)->enabled->setValue(p.value.getProperty("enabled",true));
//				}
//				else
//				{
//					childCC = new EnablingControllableContainer(p.name.toString());
//				}
//				cc->addChildControllableContainer(childCC, true, index);
//			}
//
//			childCC->editorIsCollapsed = p.value.getProperty("collapsed", false);
//			createControllablesForContainer(p.value, childCC);
//
//		}
//		else
//		{
//			Controllable* c = getControllableForJSONDefinition(p.name.toString(), p.value);
//			if (c == nullptr) continue;
//			cc->addControllable(c);
//		}
//	}
//}
//
//Controllable* PropFamily::getControllableForJSONDefinition(const String& name, var def)
//{
//	String valueType = def.getProperty("type", "").toString();
//	Controllable* c = ControllableFactory::createControllable(valueType);
//	if (c == nullptr) return nullptr;
//
//	c->setNiceName(name);
//
//	DynamicObject* d = def.getDynamicObject();
//
//	if (c->type != Controllable::TRIGGER)
//	{
//		if (def.hasProperty("min") || def.hasProperty("max")) ((Parameter*)c)->setRange(def.getProperty("min", INT32_MIN), def.getProperty("max", INT32_MAX));
//
//		if (d->hasProperty("default")) ((Parameter*)c)->setValue(d->getProperty("default"));
//
//		if (c->type == Controllable::ENUM)
//		{
//			EnumParameter* ep = dynamic_cast<EnumParameter*>(c);
//
//			if (d->hasProperty("options") && d->getProperty("options").isObject())
//			{
//				NamedValueSet options = d->getProperty("options").getDynamicObject()->getProperties();
//				for (auto& o : options)
//				{
//					ep->addOption(o.name.toString(), o.value);
//				}
//			}
//			else
//			{
//				LOG("Options property is not valid : " << d->getProperty("options").toString());
//			}
//		}
//		else if (c->type == Controllable::FLOAT)
//		{
//			FloatParameter* ep = dynamic_cast<FloatParameter*>(c);
//			if (d->hasProperty("ui"))
//			{
//				String ui = d->getProperty("ui");
//				if (ui == "slider") ep->defaultUI = FloatParameter::SLIDER;
//				else if (ui == "stepper") ep->defaultUI = FloatParameter::STEPPER;
//				else if (ui == "label") ep->defaultUI = FloatParameter::LABEL;
//				else if (ui == "time") ep->defaultUI = FloatParameter::TIME;
//			}
//		}
//
//		if (d->hasProperty("readOnly"))
//		{
//			c->setControllableFeedbackOnly(d->getProperty("readOnly"));
//		}
//
//		if (d->hasProperty("shortName"))
//		{
//			c->setCustomShortName(d->getProperty("shortName").toString());
//		}
//
//		if (d->hasProperty("description"))
//		{
//			c->description = d->getProperty("description").toString();
//		}
//
//		if (d->hasProperty("itemControlAddress"))
//		{
//			controllableItemTargetMap.set(c, d->getProperty("itemControlAddress"));
//		}
//
//	}
//
//	return c;
//}
//
//void PropFamily::registerProp(Prop* p)
//{
//	props.addIfNotAlreadyThere(p);
//
//}
//
//void PropFamily::unregisterProp(Prop* p)
//{
//	if (Engine::mainEngine->isClearing) return;
//	props.removeAllInstancesOf(p);
//}
//
//
//void PropFamily::onContainerParameterChanged(Parameter* p)
//{
//	ControllableContainer::onContainerParameterChanged(p);
//	setItemsFromControllable(p);
//}
//
//void PropFamily::onControllableFeedbackUpdate(ControllableContainer* cc, Controllable* c)
//{
//	ControllableContainer::onControllableFeedbackUpdate(cc, c);
//	setItemsFromControllable(c);
//}
//
//void PropFamily::setItemsFromControllable(Controllable* c)
//{
//	if (!controllableItemTargetMap.contains(c)) return;
//	for (auto& prop : props)
//	{
//		Controllable* pc = prop->getControllableForAddress(controllableItemTargetMap[c]);
//		if (pc != nullptr)
//		{
//			if (c->type == Controllable::TRIGGER) ((Trigger*)pc)->trigger();
//			else ((Parameter*)pc)->setValue(((Parameter*)c)->value);
//		}
//	}
//}
