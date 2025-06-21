/*
  ==============================================================================

    EmbeddedScriptManagerUI.cpp
    Created: 7 Nov 2021 11:16:05am
    Author:  bkupe

  ==============================================================================
*/

#include "EmbeddedScriptManagerUI.h"

EmbeddedScriptManagerUI::EmbeddedScriptManagerUI() :
    ManagerShapeShifterUI(EmbeddedScriptManager::getInstance()->niceName, EmbeddedScriptManager::getInstance())
{
    addExistingItems();
}

EmbeddedScriptManagerUI::~EmbeddedScriptManagerUI()
{
}
