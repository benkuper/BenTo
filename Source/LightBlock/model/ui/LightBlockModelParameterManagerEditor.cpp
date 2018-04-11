/*
  ==============================================================================

    LightBlockModelParameterManagerEditor.cpp
    Created: 10 Apr 2018 7:55:28pm
    Author:  Ben

  ==============================================================================
*/

#include "LightBlockModelParameterManagerEditor.h"

LightBlockModelParameterManagerEditor::LightBlockModelParameterManagerEditor(LightBlockModelParameterManager * manager, bool isRoot) :
	GenericManagerEditor(manager, isRoot),
	pManager(manager)
{
}

void LightBlockModelParameterManagerEditor::showMenuAndAddItem(bool)
{

	PopupMenu p;
	p.addItem(1, "Add Int Argument");
	p.addItem(2, "Add Float Argument");
	p.addItem(3, "Add String Argument");
	p.addItem(4, "Add Boolean Argument");
	p.addItem(5, "Add Color Argument");

	int result = p.show();

	switch (result)
	{
	case 1:
		pManager->addItemFromType(Parameter::INT, var(), false);
		break;
	case 2:
		pManager->addItemFromType(Parameter::FLOAT, var(), false);
		break;
	case 3:
		pManager->addItemFromType(Parameter::STRING, var(), false);
		break;
	case 4:
		pManager->addItemFromType(Parameter::BOOL, var(), false);
		break;
	case 5:
		pManager->addItemFromType(Parameter::COLOR, var(), false);
		break;
	}
}