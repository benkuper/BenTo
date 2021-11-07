/*
  ==============================================================================

    WasmManagerUI.h
    Created: 7 Nov 2021 11:16:05am
    Author:  bkupe

  ==============================================================================
*/

#pragma once

#include "../WasmManager.h"

class WasmManagerUI :
    public BaseManagerShapeShifterUI<WasmManager, WasmScript, BaseItemUI<WasmScript>>
{
public:
    WasmManagerUI();
    ~WasmManagerUI();


    static WasmManagerUI* create(const String& name) { return new WasmManagerUI(); }
};