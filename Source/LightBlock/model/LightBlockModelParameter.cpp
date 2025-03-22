/*
  ==============================================================================

    LightBlockModelParameter.cpp
    Created: 10 Apr 2018 7:49:54pm
    Author:  Ben

  ==============================================================================
*/

LightBlockModelParameter::LightBlockModelParameter(const String &name, Parameter * _p) :
	BaseItem(name, false),
	param(_p)
{
	editorCanBeCollapsed = false;


	isSelectable = false;
	param->isCustomizableByUser = true;
	param->saveValueOnly = false;

	jassert(param != nullptr);
	addControllable(param);

	editable = addBoolParameter("Editable", "If uncheck, argument will always be used at its default value, and not visible in the command editor", true);
	editable->hideInEditor = true;
}

InspectableEditor * LightBlockModelParameter::getEditorInternal(bool isRoot, Array<Inspectable*> inspectables)
{
	return new LightBlockModelParameterEditor(this, isRoot);
}

var LightBlockModelParameter::getJSONData(bool includeNonOverriden)
{
	var data = BaseItem::getJSONData(includeNonOverriden);
	data.getDynamicObject()->setProperty("param", param->getJSONData());
	return data;
}

void LightBlockModelParameter::loadJSONDataInternal(var data)
{
	BaseItem::loadJSONDataInternal(data);
	param->loadJSONData(data.getProperty("param", var()));
}

void LightBlockModelParameter::onContainerNiceNameChanged()
{
	param->setNiceName(niceName);
}
