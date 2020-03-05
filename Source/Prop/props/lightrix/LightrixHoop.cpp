/*
  ==============================================================================

    LightrixHoop.cpp
    Created: 12 Dec 2019 10:03:27am
    Author:  bkupe

  ==============================================================================
*/

#include "LightrixHoop.h"

LightrixHoopProp::LightrixHoopProp(var params) :
	BentoProp(getTypeString(), "Lightrix", params),
	buttonCC("Button")
{


	button = buttonCC.addBoolParameter("Pressed", "Is the button on the prop pressed ?", false);
	button->setControllableFeedbackOnly(true);

	shortPress = buttonCC.addTrigger("Short Press", "Short press detection");
	longPress = buttonCC.addTrigger("Long Press", "Short press detection");
	multiPress = buttonCC.addIntParameter("MultiPress", "MultiPress detection", 0);
	sensorsCC.addChildControllableContainer(&buttonCC);

	type->setValueWithData(HOOP);
	resolution->setValue(250);
}

LightrixHoopProp::~LightrixHoopProp()
{
}