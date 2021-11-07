/*
  ==============================================================================

    WasmManagerUI.cpp
    Created: 7 Nov 2021 11:16:05am
    Author:  bkupe

  ==============================================================================
*/

#include "WasmManagerUI.h"

WasmManagerUI::WasmManagerUI() :
    BaseManagerShapeShifterUI(WasmManager::getInstance()->niceName, WasmManager::getInstance())
{
    addExistingItems();
}

WasmManagerUI::~WasmManagerUI()
{
}
